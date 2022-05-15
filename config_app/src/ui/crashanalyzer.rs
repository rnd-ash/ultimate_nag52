use std::{
    fs::File,
    io::{Read, Write},
    sync::{
        atomic::{AtomicBool, AtomicU32, Ordering},
        Arc, Mutex, RwLock,
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

#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub enum ReadState {
    None,
    Prepare,
    ReadingBlock { id: u32, out_of: u32, bytes_written: u32 },
    Completed,
    Aborted(String)
}

impl ReadState {
    pub fn is_done(&self) -> bool {
        match self {
            ReadState::None => true,
            ReadState::Prepare => false,
            ReadState::ReadingBlock { id, out_of, bytes_written } => false,
            ReadState::Completed => true,
            ReadState::Aborted(_) => true,
        }
    }
}

pub struct CrashAnalyzerUI {
    server: Arc<Mutex<Kwp2000DiagnosticServer>>,
    read_state: Arc<RwLock<ReadState>>,
}

impl CrashAnalyzerUI {
    pub fn new(server:Arc<Mutex<Kwp2000DiagnosticServer>>) -> Self {
        Self {
            server,
            read_state: Arc::new(RwLock::new(ReadState::None)),
        }
    }
}

/// Return structure
/// 1. Coredump offset
/// 2. Coredump size
/// 3. Block size
fn init_flash_mode(server: &mut Kwp2000DiagnosticServer) -> DiagServerResult<(u32, u32, u32)> {
    server.set_diagnostic_session_mode(SessionType::Reprogramming)?;

    // First request coredump info
    let mut res = server.read_custom_local_identifier(0x24)?;
    if res.len() != 8 {
        return Err(DiagError::InvalidResponseLength)
    }
    let address = u32::from_le_bytes(res[0..4].try_into().unwrap());
    let size = u32::from_le_bytes(res[4..8].try_into().unwrap());
    if size == 0 {
        return Ok((0,0,0));
    }
    let mut upload_req = vec![0x35, 0x31, 0x00, 0x00, 0x00];
    upload_req.push((size >> 16) as u8);
    upload_req.push((size >> 8) as u8);
    upload_req.push((size >> 0) as u8);
    res = server.send_byte_array_with_response(&upload_req)?;
    if res.len() != 3 {
        return Err(DiagError::InvalidResponseLength)
    }
    let bs: u32 = ((res[1] as u32) << 8) | res[2] as u32;
    Ok((address, size, bs))
}

fn on_flash_end(server: &mut Kwp2000DiagnosticServer, read: Vec<u8>) -> DiagServerResult<()> {
    server.send_byte_array_with_response(&[0x37])?;
    File::create("dump.elf").unwrap().write_all(&read[20..]); // First 20 bytes are header of partition. We don't need it
    Ok(())
}

impl InterfacePage for CrashAnalyzerUI {
    fn make_ui(
        &mut self,
        ui: &mut egui::Ui,
        frame: &epi::Frame,
    ) -> crate::window::PageAction {
        ui.heading("Crash Analyzer");
        ui.label(
            RichText::new("Caution! Only use when car is off").color(Color32::from_rgb(255, 0, 0)),
        );
        let state = self.read_state.read().unwrap().clone();
        if state.is_done() {
            if ui.button("Read coredump ELF").clicked() {
                let c = self.server.clone();
                let state_c = self.read_state.clone();
                std::thread::spawn(move|| {
                    let mut lock = c.lock().unwrap();
                    *state_c.write().unwrap() = ReadState::Prepare;
                    match init_flash_mode(&mut lock.deref_mut()) {
                        Err(e) => {
                            *state_c.write().unwrap() = ReadState::Aborted(format!("ECU rejected flash programming mode: {}", e))
                        },
                        Ok(size) => {
                            println!("OK {:?}", size);
                            if size.1 == 0x00 {
                                println!("No coredump on flash");
                                *state_c.write().unwrap() = ReadState::Completed;
                            } else {
                                println!("ESP Coredump found. Will read from address 0x{:08X} {} bytes in {} byte segments", size.0, size.1, size.2);
                                let block_count = size.1 / size.2;
                                let mut data: Vec<u8> = Vec::with_capacity(size.1 as usize);
                                let mut i = 0;
                                while (data.len() as u32) < size.1 {
                                    match lock.send_byte_array_with_response(&[0x36, ((i+1) & 0xFF) as u8]) {
                                        Ok(p) => {
                                            data.extend_from_slice(&p[2..]);
                                            i+=1;
                                            *state_c.write().unwrap() = ReadState::ReadingBlock { id: i+1, out_of: block_count, bytes_written: data.len() as u32 };
                                        },
                                        Err(e) => {
                                            *state_c.write().unwrap() = ReadState::Aborted(format!("ECU rejected transfer data: {}", e));
                                            return;
                                        }
                                    }
                                }
                                on_flash_end(&mut lock.deref_mut(), data);
                            }
                            *state_c.write().unwrap() = ReadState::Completed;
                        }
                    }
                });
            }
        } else {
            ui.label("Coredump reading in progress...");
            ui.label("DO NOT EXIT THE APP");
            ui.ctx().request_repaint();
        }

        match &state {
            ReadState::None => {},
            ReadState::Prepare => {
                egui::widgets::ProgressBar::new(0.0).show_percentage().animate(true).desired_width(300.0).ui(ui);
                ui.label("Preparing ECU...");
            },
            ReadState::ReadingBlock { id, out_of, bytes_written } => {
                egui::widgets::ProgressBar::new((*id as f32) / (*out_of as f32)).show_percentage().animate(true).desired_width(300.0).ui(ui);
                ui.label(format!("Bytes read: {}", bytes_written));
            },
            ReadState::Completed => {
                ui.label(RichText::new("Coredump ELF saved as dump.elf!").color(Color32::from_rgb(0, 255, 0)));
            },
            ReadState::Aborted(r) => {
                ui.label(RichText::new(format!("Coredump read ABORTED! Reason: {}", r)).color(Color32::from_rgb(255, 0, 0)));
            },
        }
        return PageAction::SetBackButtonState(true);
    }

    fn get_title(&self) -> &'static str {
        "Ultimate-Nag52 coredump analyzer"
    }

    fn get_status_bar(&self) -> Option<Box<dyn crate::window::StatusBar>> {
        None
    }
}
