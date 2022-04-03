use std::{
    fs::File,
    io::Read,
    sync::{
        atomic::{AtomicBool, AtomicU32, Ordering},
        Arc, Mutex,
    },
};

use egui::*;
use epi::*;
use nfd::Response;

use crate::{
    usb_hw::{diag_usb::Nag52USB, flasher},
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
    usb: Arc<Mutex<Nag52USB>>,
    elf_path: Option<String>,
    flashing: Arc<AtomicBool>,
    info_data: FlashDataFormatted,
    progress: Arc<AtomicU32>,
    //flash_err: Arc<Mutex<Option<std::result::Result<(), espflash_un52::Error>>>>,
    reconnect: Arc<AtomicBool>,
}

pub struct FlasherMutate {}

impl FwUpdateUI {
    pub fn new(arc: Arc<Mutex<Nag52USB>>) -> Self {
        Self {
            usb: arc,
            elf_path: None,
            flashing: Arc::new(AtomicBool::new(false)),
            info_data: FlashDataFormatted::default(),
            progress: Arc::new(AtomicU32::new(0)),
            //flash_err: Arc::new(Mutex::new(None)),
            reconnect: Arc::new(AtomicBool::new(false)),
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
        if ui.button("Select ELF file").clicked() {
            match nfd::open_file_dialog(Some("elf"), None) {
                Ok(f) => {
                    if let Response::Okay(path) = f {
                        self.elf_path = Some(path);
                    }
                }
                Err(_) => {}
            }
        }
        if let Some(path) = &self.elf_path {
            ui.label(RichText::new(format!("Firmware path: {}", path)));
            if !self.flashing.load(Ordering::Relaxed) {
                if ui.button("Flash firmware").clicked() {
                    let elf = flasher::elf_file::EspElfFile::new(&self.elf_path.clone().unwrap());
                    if let Err(e) = elf {
                        println!("ELF ERROR {}", e);
                    }
                    let port = self.usb.lock().unwrap().on_flash_begin();
                    let mut loader = flasher::esp_loader::EspLoader::new(port);
                    loader.connect(10);
                    /*
                    match espflash_un52::Flasher::connect(port, info, Some(115200)) {
                        Ok(mut flasher) => {
                            let mut tmp = Vec::new();
                            let mut f = File::open(self.elf_path.clone().unwrap()).unwrap();
                            f.read_to_end(&mut tmp).unwrap();
                            // Flasher thread

                            let flashing_t = self.flashing.clone();
                            let flashing_err_t = self.flash_err.clone();
                            let reconnect_t = self.reconnect.clone();
                            match espflash_un52::FirmwareImage::from_data(&tmp) {
                                Err(e) => *self.flash_err.lock().unwrap() = Some(Err(e)),
                                Ok(f) => {
                                    std::thread::spawn(move || {
                                        flashing_t.store(true, Ordering::Relaxed);
                                        *flashing_err_t.lock().unwrap() =
                                            Some(flasher.load_elf_to_flash(&tmp, None, None));
                                        flashing_t.store(false, Ordering::Relaxed);
                                        reconnect_t.store(true, Ordering::Relaxed);
                                        //std::mem::drop(flasher);
                                    });
                                }
                            }
                        }
                        Err(e) => {
                            eprintln!("Cannot create ESP flasher: {}", e);
                            *self.flash_err.lock().unwrap() = Some(Err(e))
                        }
                    }
                    */
                }
                /*
                if let Some(res) = self.flash_err.lock().unwrap().as_ref() {
                    match res {
                        Ok(_) => ui.label(
                            RichText::new("Flashing completed OK!")
                                .color(Color32::from_rgb(0, 255, 0)),
                        ),
                        Err(e) => ui.label(
                            RichText::new(format!("Flashing failed! Error: {}", e))
                                .color(Color32::from_rgb(255, 0, 0)),
                        ),
                    };
                    if self.reconnect.load(Ordering::Relaxed) {
                        if self.usb.lock().unwrap().on_flash_end().is_ok() {
                            println!("OK!")
                        }
                        self.reconnect.store(false, Ordering::Relaxed);
                    }
                }
                */
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
