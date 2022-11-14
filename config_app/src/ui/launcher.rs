use std::{
    ops::RangeInclusive,
    sync::{mpsc, Arc, Mutex},
};

use ecu_diagnostics::{
    channel::IsoTPChannel,
    hardware::{
        passthru::{PassthruDevice, PassthruScanner},
        Hardware, HardwareResult, HardwareScanner,
    },
};
use eframe::egui;
use eframe::egui::*;

#[cfg(unix)]
use ecu_diagnostics::hardware::socketcan::{SocketCanDevice, SocketCanScanner};

use crate::{
    ui::main::MainPage,
    usb_hw::{
        diag_usb::{EspLogMessage, Nag52USB},
        scanner::Nag52UsbScanner,
    },
    window::{InterfacePage, PageAction},
};

use super::widgets::range_display::range_display;

type ScanResult = std::result::Result<Vec<String>, String>;

pub struct Launcher {
    selected: String,
    old_selected: String,
    launch_err: Option<String>,
    usb_scanner: Nag52UsbScanner,
    pt_scanner: PassthruScanner,
    #[cfg(unix)]
    scan_scanner: SocketCanScanner,
    selected_device: String,
    curr_api_type: DeviceType,
}

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord)]
pub enum DeviceType {
    Passthru,
    #[cfg(unix)]
    SocketCan,
    Usb,
}

pub enum DynamicDevice {
    Passthru(Arc<Mutex<PassthruDevice>>),
    Usb(Arc<Mutex<Nag52USB>>),
    #[cfg(unix)]
    SocketCAN(Arc<Mutex<SocketCanDevice>>),
}

impl DynamicDevice {
    pub fn get_logger(&mut self) -> Option<mpsc::Receiver<EspLogMessage>> {
        match self {
            DynamicDevice::Usb(usb) => usb.lock().unwrap().get_logger_receiver(),
            _ => None,
        }
    }

    pub fn create_isotp_channel(&mut self) -> HardwareResult<Box<dyn IsoTPChannel>> {
        match self {
            DynamicDevice::Passthru(pt) => {
                PassthruDevice::toggle_sw_channel_raw(pt, true);
                Hardware::create_iso_tp_channel(pt.clone())
            }
            DynamicDevice::Usb(usb) => Hardware::create_iso_tp_channel(usb.clone()),
            #[cfg(unix)]
            DynamicDevice::SocketCAN(s) => Hardware::create_iso_tp_channel(s.clone()),
        }
    }
}

impl Launcher {
    pub fn new() -> Self {
        Self {
            selected: "".into(),
            old_selected: "".into(),
            launch_err: None,
            usb_scanner: Nag52UsbScanner::new(),
            pt_scanner: PassthruScanner::new(),
            #[cfg(unix)]
            scan_scanner: SocketCanScanner::new(),
            selected_device: String::new(),
            curr_api_type: DeviceType::Usb,
        }
    }
}

impl Launcher {
    pub fn open_device(&self, name: &str) -> HardwareResult<DynamicDevice> {
        let mut tmp = name.to_string();
        if tmp.contains(" API") {
            tmp = tmp.split(" API").next().unwrap().to_string();
        }
        println!("Opening '{}'", tmp);
        Ok(match self.curr_api_type {
            DeviceType::Passthru => {
                DynamicDevice::Passthru(self.pt_scanner.open_device_by_name(&tmp)?)
            }
            #[cfg(unix)]
            DeviceType::SocketCan => {
                DynamicDevice::SocketCAN(self.scan_scanner.open_device_by_name(&tmp)?)
            }
            DeviceType::Usb => DynamicDevice::Usb(self.usb_scanner.open_device_by_name(&tmp)?),
        })
    }

    pub fn get_device_list<T, X: Hardware>(scanner: &T) -> Vec<String>
    where
        T: HardwareScanner<X>,
    {
        return scanner
            .list_devices()
            .iter()
            .map(|x| {
                let mut s = x.name.clone();
                if let Some(api) = &x.api_version {
                    s.push_str(&format!(" API {}", api));
                }
                s
            })
            .collect();
    }
}

impl InterfacePage for Launcher {
    fn make_ui(&mut self, ui: &mut Ui, frame: &eframe::Frame) -> crate::window::PageAction {
        ui.label("Ultimate-Nag52 configuration utility!");
        ui.label(
            "Please plug in your TCM via USB and select the correct port, or select another API",
        );

        ui.radio_value(&mut self.curr_api_type, DeviceType::Usb, "USB connection");
        ui.radio_value(
            &mut self.curr_api_type,
            DeviceType::Passthru,
            "Passthru OBD adapter",
        );
        #[cfg(unix)]
        {
            ui.radio_value(
                &mut self.curr_api_type,
                DeviceType::SocketCan,
                "SocketCAN device",
            );
        }
        ui.heading("Devices");

        let dev_list = match self.curr_api_type {
            DeviceType::Passthru => Self::get_device_list(&self.pt_scanner),
            #[cfg(unix)]
            DeviceType::SocketCan => Self::get_device_list(&self.scan_scanner),
            DeviceType::Usb => Self::get_device_list(&self.usb_scanner),
        };

        if dev_list.len() == 0 {
        } else {
            egui::ComboBox::from_label("Select device")
                .width(400.0)
                .selected_text(&self.selected_device)
                .show_ui(ui, |cb_ui| {
                    for dev in dev_list {
                        cb_ui.selectable_value(&mut self.selected_device, dev.clone(), dev);
                    }
                });
        }

        if !self.selected_device.is_empty() && ui.button("Launch configuration app").clicked() {
            match self.open_device(&self.selected_device) {
                Ok(mut dev) => {
                    if let Ok(channel) = dev.create_isotp_channel() {
                        return PageAction::Overwrite(Box::new(MainPage::new(
                            channel,
                            dev.get_logger(),
                            self.selected_device.clone(),
                        )));
                    }
                }
                Err(e) => self.launch_err = Some(format!("Cannot open device: {}", e)),
            }
        }

        if ui.button("Refresh device list").clicked() {
            self.pt_scanner = PassthruScanner::new();
            self.usb_scanner = Nag52UsbScanner::new();
            #[cfg(unix)]
            {
                self.scan_scanner = SocketCanScanner::new();
            }
            self.selected_device.clear();
        }

        if let Some(e) = &self.launch_err {
            ui.label(RichText::new(format!("Error: {}", e)).color(Color32::from_rgb(255, 0, 0)));
        }

        range_display(ui, 65.0, 50.0, 70.0, 0.0, 100.0);

        crate::window::PageAction::None
    }

    fn get_title(&self) -> &'static str {
        "Ultimate-NAG52 configuration utility (Launcher)"
    }

    fn get_status_bar(&self) -> Option<Box<dyn crate::window::StatusBar>> {
        None
    }
}
