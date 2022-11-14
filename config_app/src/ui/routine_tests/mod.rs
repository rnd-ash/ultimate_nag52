use std::sync::{Arc, Mutex};

use ecu_diagnostics::kwp2000::Kwp2000DiagnosticServer;

use crate::window::PageAction;

use self::solenoid_test::SolenoidTestPage;

use super::status_bar::MainStatusBar;

pub mod solenoid_test;

pub struct RoutinePage {
    bar: MainStatusBar,
    server: Arc<Mutex<Kwp2000DiagnosticServer>>,
}

impl RoutinePage {
    pub fn new(server: Arc<Mutex<Kwp2000DiagnosticServer>>, bar: MainStatusBar) -> Self {
        Self { bar, server }
    }
}

impl crate::window::InterfacePage for RoutinePage {
    fn make_ui(
        &mut self,
        ui: &mut eframe::egui::Ui,
        frame: &eframe::Frame,
    ) -> crate::window::PageAction {
        ui.heading("Diagnostic routines");

        ui.label(
            "
            Select test routine to run
        ",
        );

        let mut page_action = PageAction::None;

        if ui.button("Solenoid test").clicked() {
            page_action = PageAction::Add(Box::new(SolenoidTestPage::new(
                self.server.clone(),
                self.bar.clone(),
            )));
        }

        page_action
    }

    fn get_title(&self) -> &'static str {
        "Routine executor"
    }

    fn get_status_bar(&self) -> Option<Box<dyn crate::window::StatusBar>> {
        Some(Box::new(self.bar.clone()))
    }
}
