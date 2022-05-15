use egui::Color32;

use crate::window::InterfacePage;

pub mod firmware_update;
pub mod launcher;
pub mod main;
pub mod status_bar;
pub mod diagnostics;
pub mod configuration;
pub mod crashanalyzer;
pub mod kwp_event;

#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub enum StatusText {
    Ok(String),
    Err(String)
}

impl egui::Widget for StatusText {
    fn ui(self, ui: &mut egui::Ui) -> egui::Response {
        match self {
            StatusText::Ok(t) => ui.add(egui::Label::new(egui::RichText::new(t))),
            StatusText::Err(t) => ui.add(egui::Label::new(egui::RichText::new(t).color(Color32::from_rgb(255, 0, 0)))),
        }
    }
}