use std::sync::{Arc, Mutex};

use ecu_diagnostics::{
    hardware::Hardware,
    DiagnosticServer,
    kwp2000::{self, Kwp2000DiagnosticServer, Kwp2000ServerOptions, Kwp2000VoidHandler},
};
use egui::*;
use epi::Frame;

use crate::{
    usb_hw::diag_usb::Nag52USB,
    window::{InterfacePage, PageAction, StatusBar},
};

use super::{firmware_update::FwUpdateUI, status_bar::MainStatusBar};

use ecu_diagnostics::kwp2000::*;
use crate::ui::diagnostics::DiagnosticsPage;

pub struct MainPage {
    dev: Arc<Mutex<Nag52USB>>,
    bar: MainStatusBar,
    show_about_ui: bool,
}

impl MainPage {
    pub fn new(dev: Arc<Mutex<Nag52USB>>) -> Self {
        Self {
            dev: dev.clone(),
            bar: MainStatusBar::new(dev),
            show_about_ui: false,
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
                    self.show_about_ui = true;
                }
            })
        });
        ui.add(egui::Separator::default());

        let mut create_page = None;
        ui.vertical(|v| {
            v.heading("Utilities");
            if v.button("Firmware updater").on_disabled_hover_ui(|u| {u.label("Broken, will be added soon!");}).clicked() {
                create_page = Some(PageAction::Add(Box::new(FwUpdateUI::new(self.dev.clone()))));
            }
            if v.button("Diagnostics").clicked() {
                create_page = Some(PageAction::Add(Box::new(DiagnosticsPage::new(self.dev.clone(), self.bar.clone()))));
            }
            if v.button("Map tuner").clicked() {}
            if v.button("Configure drive profiles").clicked() {}
            if v.button("Configure vehicle / gearbox").clicked() {}
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
                        about_cols.label(format!("TCM firmware version: UNKNOWN"));
                        about_cols.label(format!("TCM firmware build: UNKNOWN"));
                        about_cols.label(format!("TCM EGS compatibility mode: EGS52"));
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
