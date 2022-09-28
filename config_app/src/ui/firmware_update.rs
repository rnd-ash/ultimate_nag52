use std::{
    sync::{
        Arc, Mutex, RwLock,
    }, ops::DerefMut, time::Instant,
};

use ecu_diagnostics::{kwp2000::{Kwp2000DiagnosticServer, SessionType, ResetMode}, DiagServerResult, DiagnosticServer, DiagError};
use eframe::egui::*;
use eframe::egui;
use nfd::Response;

use crate::{
    usb_hw::flasher::{bin::{Firmware, load_binary, FirmwareHeader}},
    window::{InterfacePage, PageAction},
};

#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub enum FlashState {
    None,
    Prepare,
    WritingBlock { id: u32, out_of: u32, bytes_written: u32 },
    Verify,
    Completed,
    Aborted(String)
}

impl FlashState {
    pub fn is_done(&self) -> bool {
        match self {
            FlashState::None => true,
            FlashState::Prepare => false,
            FlashState::WritingBlock { id, out_of, bytes_written } => false,
            FlashState::Verify => false,
            FlashState::Completed => true,
            FlashState::Aborted(_) => true,
        }
    }
}

pub struct FwUpdateUI {
    server: Arc<Mutex<Kwp2000DiagnosticServer>>,
    elf_path: Option<String>,
    firmware: Option<Firmware>,
    flash_state: Arc<RwLock<FlashState>>,
    flash_start: Instant,
    flash_measure: Instant,
    flash_speed: u32,
    flash_eta: u32,
    curr_fw: Option<FirmwareHeader>
}

pub struct FlasherMutate {}

impl FwUpdateUI {
    pub fn new(server:Arc<Mutex<Kwp2000DiagnosticServer>>) -> Self {
        Self {
            server,
            elf_path: None,
            firmware: None,
            flash_state: Arc::new(RwLock::new(FlashState::None)),
            flash_start: Instant::now(),
            flash_measure: Instant::now(),
            flash_speed: 0,
            flash_eta: 0,
            curr_fw: None
        }
    }
}

fn init_flash_mode(server: &mut Kwp2000DiagnosticServer, flash_size: u32) -> DiagServerResult<u32> {
    server.set_diagnostic_session_mode(SessionType::Reprogramming)?;
    let mut req: Vec<u8> = vec![0x34, 0x00, 0x00, 0x00, 0x00];
    req.push((flash_size >> 16) as u8);
    req.push((flash_size >> 8 ) as u8);
    req.push((flash_size) as u8);
    let resp = server.send_byte_array_with_response(&req)?;
    let bs = (resp[1] as u16) << 8 | resp[2] as u16;
    Ok(bs as u32)
}

fn on_flash_end(server: &mut Kwp2000DiagnosticServer) -> DiagServerResult<()> {
    server.send_byte_array_with_response(&[0x37])?;
    let status = server.send_byte_array_with_response(&[0x31, 0xE1])?;
    if status[2] == 0x00 {
        eprintln!("ECU Flash check OK! Rebooting");
        return server.reset_ecu(ResetMode::PowerOnReset)
    } else {
        eprintln!("ECU Flash check failed :(");
        return Err(DiagError::NotSupported)
    }
}

impl InterfacePage for FwUpdateUI {
    fn make_ui(
        &mut self,
        ui: &mut egui::Ui,
        frame: &eframe::Frame,
    ) -> crate::window::PageAction {
        ui.heading("Firmware update");
        ui.label(
            RichText::new("Caution! Only use when car is off").color(Color32::from_rgb(255, 0, 0)),
        );
        if ui.button("Select BIN firmware file").clicked() {
            match nfd::open_file_dialog(Some("bin"), None) {
                Ok(f) => {
                    if let Response::Okay(path) = f {
                        self.elf_path = Some(path.clone());
                        match load_binary(path) {
                            Ok(f) => self.firmware = Some(f),
                            Err(e) => {
                                eprintln!("E loading binary! {}", e)
                            }
                        }
                    }
                }
                Err(_) => {}
            }
        }
        if let Some(firmware) = &self.firmware {
            ui.heading("New firmware");
            ui.label(RichText::new(format!(
"Firmware size: {} bytes

Firmware type: {}
Version: {}
IDF Version: {}
Compile time: {} on {}
",

firmware.raw.len(),
firmware.header.get_fw_name(),
firmware.header.get_version(),
firmware.header.get_idf_version(),
firmware.header.get_time(),
firmware.header.get_date()
            )));

            if let Some(c_fw) = self.curr_fw {
                ui.heading("Current firmware");
                ui.label(RichText::new(format!(
"
Firmware type: {}
Version: {}
IDF Version: {}
Compile time: {} on {}
",

c_fw.get_fw_name(),
c_fw.get_version(),
c_fw.get_idf_version(),
c_fw.get_time(),
c_fw.get_date()
                )));
            }

            let state = self.flash_state.read().unwrap().clone();
            if state.is_done() {
                if ui.button("Query current firmware").clicked() {
                    let mut lock = self.server.lock().unwrap();
                    match lock.read_custom_local_identifier(0x28)
                        .and_then(|resp| {
                            if resp.len() != std::mem::size_of::<FirmwareHeader>() {
                                Err(DiagError::InvalidResponseLength)
                            } else {
                                Ok(unsafe { std::ptr::read::<FirmwareHeader>(resp.as_ptr() as *const _ ) })
                            }
                        } ) {
                            Ok(header) => {
                                self.curr_fw = Some(header)
                            },
                            Err(e) => {
                                // TODO
                            }
                    }

                }
                if ui.button("Flash firmware").clicked() {
                    let c = self.server.clone();
                    let state_c = self.flash_state.clone();
                    let fw = firmware.clone();
                    let mut old_timeouts = (0, 0);
                    std::thread::spawn(move|| {
                        let mut lock = c.lock().unwrap();
                        *state_c.write().unwrap() = FlashState::Prepare;
                        old_timeouts = (lock.get_read_timeout(), lock.get_write_timeout());
                        lock.set_rw_timeout(10000, 10000);

                        match init_flash_mode(&mut lock.deref_mut(), fw.raw.len() as u32) {
                            Err(e) => {
                                lock.set_rw_timeout(old_timeouts.0, old_timeouts.1);
                                *state_c.write().unwrap() = FlashState::Aborted(format!("ECU rejected flash programming mode: {}", e))
                            },
                            Ok(size) => {
                                // Start the flasher!
                                let arr = fw.raw.chunks(size as usize);
                                let total_blocks = arr.len() as u32;
                                let mut failure = false;
                                let mut bytes_written = 0;
                                for (block_id, block) in arr.enumerate() {
                                    let mut req = vec![0x36, ((block_id+1) & 0xFF) as u8]; // [Transfer data request, block counter]
                                    req.extend_from_slice(block); // Block to transfer
                                    if let Err(e) = lock.send_byte_array_with_response(&req) {
                                        *state_c.write().unwrap() = FlashState::Aborted(format!("ECU failed to write data to flash: {}", e));
                                        eprintln!("Writing failed! Error {}", e);
                                        lock.set_rw_timeout(old_timeouts.0, old_timeouts.1);
                                        failure = true;
                                        break;
                                    } else {
                                        bytes_written += block.len() as u32;
                                        *state_c.write().unwrap() = FlashState::WritingBlock { id: (block_id+1) as u32, out_of: total_blocks, bytes_written }
                                    }
                                }
                                if !failure {
                                    *state_c.write().unwrap() = FlashState::Verify;
                                    if let Err(e) = on_flash_end(&mut lock) {
                                        lock.set_rw_timeout(old_timeouts.0, old_timeouts.1);
                                        *state_c.write().unwrap() = FlashState::Aborted(format!("ECU failed to verify flash: {}", e))
                                    } else {
                                        *state_c.write().unwrap() = FlashState::Completed;
                                    }
                                }
                                lock.set_rw_timeout(old_timeouts.0, old_timeouts.1);
                            }
                        }
                    });
                }
            } else {
                ui.label("Flashing in progress...");
                ui.label("DO NOT EXIT THE APP");
            }
            match &state {
                FlashState::None => {},
                FlashState::Prepare => {
                    egui::widgets::ProgressBar::new(0.0).show_percentage().animate(true).desired_width(300.0).ui(ui);
                    ui.label("Preparing ECU...");
                    self.flash_start = Instant::now();
                    self.flash_measure = Instant::now();
                    self.flash_speed = 0;
                    self.flash_eta = 0;
                },
                FlashState::WritingBlock { id, out_of, bytes_written } => {
                    egui::widgets::ProgressBar::new((*id as f32)/(*out_of as f32)).show_percentage().desired_width(300.0).animate(true).ui(ui);
                    let spd = (1000.0 * *bytes_written as f32 / self.flash_start.elapsed().as_millis() as f32) as u32;
                    
                    if self.flash_measure.elapsed().as_millis() > 1000 {
                        self.flash_measure = Instant::now();
                        self.flash_speed = spd;
                        self.flash_eta = (firmware.raw.len() as u32 - *bytes_written) / spd;
                    }
                    
                    ui.label(format!("Bytes written: {}. Avg {:.0} bytes/sec", bytes_written, self.flash_speed));
                    ui.label(format!("ETA: {} seconds remaining", self.flash_eta));
                },
                FlashState::Verify => {
                    egui::widgets::ProgressBar::new(100.0).show_percentage().desired_width(300.0).ui(ui);
                    ui.label("Verifying written data...");
                },
                FlashState::Completed => {
                    ui.label(RichText::new("Flashing completed successfully!").color(Color32::from_rgb(0, 255, 0)));
                },
                FlashState::Aborted(r) => {
                    ui.label(RichText::new(format!("Flashing was ABORTED! Reason: {}", r)).color(Color32::from_rgb(255, 0, 0)));
                },
            }
            return PageAction::SetBackButtonState(state.is_done());
        }
        return PageAction::SetBackButtonState(true);
    }

    fn get_title(&self) -> &'static str {
        "Ultimate-Nag52 firmware updater"
    }

    fn get_status_bar(&self) -> Option<Box<dyn crate::window::StatusBar>> {
        None
    }
}
