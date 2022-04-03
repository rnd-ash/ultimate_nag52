use std::sync::{Arc, Mutex};

use ecu_diagnostics::hardware::{HardwareResult, HardwareScanner};
use egui::*;
use epi::*;

use crate::{
    ui::main::MainPage,
    usb_hw::{diag_usb::Nag52USB, scanner::Nag52UsbScanner},
    window::{InterfacePage, PageAction},
};

type ScanResult = std::result::Result<Vec<String>, String>;

pub struct Launcher {
    selected: String,
    old_selected: String,
    launch_err: Option<String>,
    scanner: Nag52UsbScanner,
    selected_device: String,
}

impl Launcher {
    pub fn new() -> Self {
        Self {
            selected: "".into(),
            old_selected: "".into(),
            launch_err: None,
            scanner: Nag52UsbScanner::new(),
            selected_device: String::new(),
        }
    }
}

impl Launcher {
    pub fn open_device(&self, name: &str) -> HardwareResult<Arc<Mutex<Nag52USB>>> {
        self.scanner.open_device_by_name(name)
    }

    pub fn get_device_list(&self) -> Vec<String> {
        return self
            .scanner
            .list_devices()
            .iter()
            .map(|x| x.name.clone())
            .collect();
    }
}

impl InterfacePage for Launcher {
    fn make_ui(&mut self, ui: &mut Ui, frame: &epi::Frame) -> crate::window::PageAction {
        ui.label("Ultimate-Nag52 configuration utility!");
        ui.label("Please plug in your TCM via USB and select the correct port");

        ui.heading("Devices");


        if self.get_device_list().len() == 0 {
        } else {
            egui::ComboBox::from_label("Select device")
                .width(400.0)
                .selected_text(&self.selected_device)
                .show_ui(ui, |cb_ui| {
                    for dev in self.get_device_list() {
                        cb_ui.selectable_value(&mut self.selected_device, dev.clone(), dev);
                    }
                });
        }

        if !self.selected_device.is_empty() && ui.button("Launch configuration app").clicked() {
            match self.open_device(&self.selected_device) {
                Ok(dev) => {
                    return PageAction::Overwrite(Box::new(MainPage::new(dev)));
                }
                Err(e) => self.launch_err = Some(format!("Cannot open device: {}", e)),
            }
        }

        if ui.button("Refresh device list").clicked() {
            self.scanner = Nag52UsbScanner::new();
            self.selected_device.clear();
        }

        if let Some(e) = &self.launch_err {
            ui.label(RichText::new(format!("Error: {}", e)).color(Color32::from_rgb(255, 0, 0)));
        }
        crate::window::PageAction::None
    }

    fn get_title(&self) -> &'static str {
        "Ultimate-NAG52 configuration utility (Launcher)"
    }

    fn get_status_bar(&self) -> Option<Box<dyn crate::window::StatusBar>> {
        None
    }
}
