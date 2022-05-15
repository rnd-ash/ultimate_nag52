use std::sync::{Arc, Mutex, mpsc};

use ecu_diagnostics::{
    hardware::Hardware,
    DiagnosticServer,
    kwp2000::{self, Kwp2000DiagnosticServer, Kwp2000ServerOptions, Kwp2000VoidHandler}, channel::IsoTPChannel,
};
use egui::*;
use epi::Frame;

use crate::{
    usb_hw::diag_usb::{Nag52USB, EspLogMessage},
    window::{InterfacePage, PageAction, StatusBar},
};

use super::{firmware_update::FwUpdateUI, status_bar::MainStatusBar, configuration::ConfigPage, crashanalyzer::CrashAnalyzerUI, diagnostics::solenoids::SolenoidPage, };

use ecu_diagnostics::kwp2000::*;
use crate::ui::diagnostics::DiagnosticsPage;

#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
struct DevInfo {
    pub compat_mode: String,
    pub fw_version: String,
    pub fw_date: String
}

pub struct MainPage {
    bar: MainStatusBar,
    show_about_ui: bool,
    diag_server: Arc<Mutex<Kwp2000DiagnosticServer>>,
    dev_info: DevInfo
}

impl MainPage {
    pub fn new(mut channel: Box<dyn IsoTPChannel>, logger: Option<mpsc::Receiver<EspLogMessage>>, hw_name: String) -> Self {
        let channel_cfg = ecu_diagnostics::channel::IsoTPSettings {
            block_size: 0,
            st_min: 0,
            extended_addressing: false,
            pad_frame: true,
            can_speed: 500_000,
            can_use_ext_addr: false,
        };
        let server_settings = Kwp2000ServerOptions {
            send_id: 0x07E1,
            recv_id: 0x07E9,
            read_timeout_ms: 5000,
            write_timeout_ms: 5000,
            global_tp_id: 0,
            tester_present_interval_ms: 2000,
            tester_present_require_response: true,
            global_session_control: false
        };
        let mut kwp = Kwp2000DiagnosticServer::new_over_iso_tp(
            server_settings,
            channel,
            channel_cfg,
            Kwp2000VoidHandler {},
        ).unwrap();

        Self {
            bar: MainStatusBar::new(logger, hw_name),
            show_about_ui: false,
            diag_server: Arc::new(Mutex::new(kwp)),
            dev_info: DevInfo { compat_mode: "UNKNOWN".into(), fw_version: "UNKNOWN".into(), fw_date: "UNKNOWN".into() }
        }
    }
}

impl InterfacePage for MainPage {
    fn make_ui(
        &mut self,
        ui: &mut egui::Ui,
        frame: &epi::Frame,
    ) -> crate::window::PageAction {
        // UI context menu
        egui::menu::bar(ui, |bar_ui| {
            bar_ui.menu_button("File", |x| {
                if x.button("Quit").clicked() {
                    //TODO
                }
                if x.button("About").clicked() {
                    // Query info (Update it)
                    if let Ok (info) = self.diag_server.lock().unwrap().read_daimler_mmc_identification() {
                        self.dev_info = DevInfo { 
                            compat_mode: if info.supplier == 7 { "EGS52".into() } else { "EGS53".into() }, 
                            fw_version: info.sw_version, 
                            fw_date: "UNKNOWN".into() 
                        };
                    } else {
                        self.dev_info = DevInfo { compat_mode: "UNKNOWN".into(), fw_version: "UNKNOWN".into(), fw_date: "UNKNOWN".into() };
                    }
                    self.show_about_ui = true;
                }
            })
        });
        ui.add(egui::Separator::default());
        let mut create_page = None;
        ui.vertical(|v| {
            v.heading("Utilities");
            if v.button("Firmware updater").on_disabled_hover_ui(|u| {u.label("Broken, will be added soon!");}).clicked() {
                create_page = Some(PageAction::Add(Box::new(FwUpdateUI::new(self.diag_server.clone()))));
            }
            if v.button("Crash analyzer").clicked() {
                create_page = Some(PageAction::Add(Box::new(CrashAnalyzerUI::new(self.diag_server.clone()))));
            }
            if v.button("Diagnostics").clicked() {
                create_page = Some(PageAction::Add(Box::new(DiagnosticsPage::new(self.diag_server.clone(), self.bar.clone()))));
            }
            if v.button("Solenoid live view").clicked() {
                create_page = Some(PageAction::Add(Box::new(SolenoidPage::new(self.diag_server.clone(), self.bar.clone()))));
            }
            if v.button("Map tuner").clicked() {}
            if v.button("Configure drive profiles").clicked() {}
            if v.button("Configure vehicle / gearbox").clicked() {
                create_page = Some(PageAction::Add(Box::new(ConfigPage::new(self.diag_server.clone(), self.bar.clone()))));
            }
        });
        if let Some(page) = create_page {
            return page;
        }

        if self.show_about_ui {
            egui::containers::Window::new("About")
                .resizable(false)
                .collapsible(false)
                .default_pos(&[400f32, 300f32])
                .show(ui.ctx(), |win| {
                    win.vertical(|about_cols| {
                        about_cols.heading("Version data");
                        about_cols.label(format!(
                            "Configuration app version: {}",
                            env!("CARGO_PKG_VERSION")
                        ));
                        about_cols.label(format!("TCM firmware version: {}", self.dev_info.fw_version));
                        about_cols.label(format!("TCM firmware build: {}", self.dev_info.fw_date));
                        about_cols.label(format!("TCM EGS compatibility mode: {}", self.dev_info.compat_mode));
                        about_cols.separator();
                        about_cols.heading("Open source");
                        about_cols.add(egui::Hyperlink::from_label_and_url(
                            "Github repository (Configuration utility)",
                            "https://github.com/rnd-ash/ultimate_nag52/tree/main/config_app",
                        ));
                        about_cols.add(egui::Hyperlink::from_label_and_url(
                            "Github repository (TCM source code)",
                            "https://github.com/rnd-ash/ultimate-nag52-fw",
                        ));
                        about_cols.separator();
                        about_cols.heading("Author");
                        about_cols.add(egui::Hyperlink::from_label_and_url(
                            "rnd-ash",
                            "https://github.com/rnd-ash",
                        ));
                        if about_cols.button("Close").clicked() {
                            self.show_about_ui = false;
                        }
                    })
                });
        }

        PageAction::None
    }

    fn get_title(&self) -> &'static str {
        "Ultimate-Nag52 configuration utility (Home)"
    }

    fn get_status_bar(&self) -> Option<Box<dyn crate::window::StatusBar>> {
        Some(Box::new(self.bar.clone()))
    }
}
