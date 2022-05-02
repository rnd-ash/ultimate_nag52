use std::{
    fs::File,
    io::Read,
    sync::{
        atomic::{AtomicBool, AtomicU32, Ordering},
        Arc, Mutex,
    }, num::Wrapping, ops::DerefMut,
};

use ecu_diagnostics::{kwp2000::{Kwp2000DiagnosticServer, SessionType, ResetMode}, DiagServerResult, DiagnosticServer, DiagError};
use egui::*;
use epi::*;
use nfd::Response;

use crate::{
    usb_hw::{diag_usb::Nag52USB, flasher::{self, bin::{Firmware, load_binary}}},
    window::{InterfacePage, PageAction},
};

pub struct FlashDataFormatted {
    freq: String,
    size: String,
    features: String,
    mac: String,
    revision: String,
}

impl Default for FlashDataFormatted {
    fn default() -> Self {
        Self {
            freq: "Unknown Mhz".into(),
            size: "Unknown MB".into(),
            features: "N/A".into(),
            mac: "00-00-00-00-00-00".into(),
            revision: "N/A".into(),
        }
    }
}

pub struct FwUpdateUI {
    server: Arc<Mutex<Kwp2000DiagnosticServer>>,
    elf_path: Option<String>,
    flashing: Arc<AtomicBool>,
    firmware: Option<Firmware>,
    progress: Arc<AtomicU32>,
    //flash_err: Arc<Mutex<Option<std::result::Result<(), espflash_un52::Error>>>>,
    reconnect: Arc<AtomicBool>,
}

pub struct FlasherMutate {}

impl FwUpdateUI {
    pub fn new(server:Arc<Mutex<Kwp2000DiagnosticServer>>) -> Self {
        Self {
            server,
            elf_path: None,
            flashing: Arc::new(AtomicBool::new(false)),
            firmware: None,
            progress: Arc::new(AtomicU32::new(0)),
            //flash_err: Arc::new(Mutex::new(None)),
            reconnect: Arc::new(AtomicBool::new(false)),
        }
    }

    fn init_flash_mode(&self, server: &mut Kwp2000DiagnosticServer, flash_size: u32) -> DiagServerResult<u32> {
        server.set_diagnostic_session_mode(SessionType::Reprogramming)?;
        let mut req: Vec<u8> = vec![0x34, 0x00, 0x00, 0x00, 0x00];
        req.push((flash_size >> 16) as u8);
        req.push((flash_size >> 8 ) as u8);
        req.push((flash_size) as u8);
        let resp = server.send_byte_array_with_response(&req)?;
        let bs = (resp[1] as u16) << 8 | resp[2] as u16;
        Ok(bs as u32)
    }

    fn on_flash_end(&self, server: &mut Kwp2000DiagnosticServer) -> DiagServerResult<()> {
        server.send_byte_array_with_response(&[0x37])?;
        let status = server.send_byte_array_with_response(&[0x31, 0xE1])?;
        if status[1] == 0x00 {
            eprintln!("ECU Flash check OK! Rebooting");
            return server.reset_ecu(ResetMode::PowerOnReset)
        } else {
            eprintln!("ECU Flash check failed :(");
            return Err(DiagError::NotSupported)
        }
    }
}

impl InterfacePage for FwUpdateUI {
    fn make_ui(
        &mut self,
        ui: &mut egui::Ui,
        frame: &epi::Frame,
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
            ui.label(RichText::new(format!(
"Firmware size: {} bytes

Version: {}
IDF Version: {}
Compile time: {} on {}
",
firmware.raw.len(),
firmware.header.get_version(),
firmware.header.get_idf_version(),
firmware.header.get_time(),
firmware.header.get_date()
            )));
            if !self.flashing.load(Ordering::Relaxed) {
                if ui.button("Flash firmware").clicked() {
                    let mut lock = self.server.lock().unwrap();
                    match self.init_flash_mode(&mut lock.deref_mut(), firmware.raw.len() as u32) {
                        Err(e) => eprintln!("ECU flash mode reject! {}", e),
                        Ok(size) => {
                            // Start the flasher!
                            let arr = firmware.raw.chunks(size as usize);
                            let mut failure = false;
                            for (block_id, block) in arr.enumerate() {
                                let mut req = vec![0x36, ((block_id+1) & 0xFF) as u8]; // [Transfer data request, block counter]
                                req.extend_from_slice(block); // Block to transfer
                                if let Err(e) = lock.send_byte_array_with_response(&req) {
                                    eprintln!("Writing failed! Error {}", e);
                                    failure = true;
                                    break;
                                }
                            }
                            if !failure {
                                if let Err(e) = self.on_flash_end(&mut lock) {
                                    eprintln!("ECU flash check error {}", e)
                                }
                            }
                        }
                    }
                }
            } else {
                ui.label("Flashing in progress...");
                ui.label("DO NOT EXIT THE APP");
                return PageAction::SetBackButtonState(false);
            }
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
