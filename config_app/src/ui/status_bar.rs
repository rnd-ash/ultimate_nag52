use ecu_diagnostics::hardware::Hardware;
use eframe::egui::*;
use std::{
    collections::VecDeque,
    sync::{Arc, Mutex, RwLock, mpsc}, borrow::BorrowMut,
};

use crate::{
    usb_hw::{diag_usb::{EspLogMessage, Nag52USB}, KwpEventLevel},
    window::{InterfacePage, StatusBar},
};
use eframe::egui;

#[derive(Clone)]
pub struct MainStatusBar {
    show_log_view: bool,
    msgs: VecDeque<(KwpEventLevel, String)>,
    receiver: Arc<mpsc::Receiver<(KwpEventLevel, String)>>,
    hw_name: String,
    auto_scroll: bool,
    use_light_theme: bool
}

impl MainStatusBar {
    pub fn new(logger: mpsc::Receiver<(KwpEventLevel, String)>, hw_name: String) -> Self {
        Self {
            show_log_view: false,
            msgs: VecDeque::new(),
            auto_scroll: true,
            use_light_theme: false,
            receiver: Arc::new(logger),
            hw_name
        }
    }
}

impl StatusBar for MainStatusBar {
    fn draw(&mut self, ui: &mut egui::Ui, ctx: &egui::Context) {
        ui.label(format!("Connected via {}", self.hw_name));
        if ui.button("Debug view").clicked() {
            self.show_log_view = true;
        }

        egui::widgets::global_dark_light_mode_buttons(ui);


        if self.show_log_view {
            let ui_h = ui.available_height();
            let ui_w = ui.available_width();
            egui::containers::Window::new("Debug view")
                .resizable(true)
                .default_height(ui_h/4.0)
                .default_width(ui_w/4.0)
                .show(ui.ctx(), |log_view| {
                    log_view.vertical(|l_view| {
                        let max_height = l_view.available_height();
                        egui::containers::ScrollArea::vertical()
                        .auto_shrink([false, false])
                        .max_height(500.0)
                        .stick_to_bottom(true)
                            .show(l_view, |scroll| {
                                for (lvl, msg) in &self.msgs {
                                    scroll.label(
                                        RichText::new(format!(
                                            "{}",
                                            msg
                                        ))
                                        .color(match lvl
                                        {
                                            KwpEventLevel::Warn => {
                                                Color32::from_rgb(255, 215, 0)
                                            }
                                            KwpEventLevel::Err => {
                                                Color32::from_rgb(255, 0, 0)
                                            }
                                            _ => ui.visuals().text_color()
                                        }),
                                    );
                                }
                            });
                        while let Ok(msg) = self.receiver.try_recv() {
                            if self.msgs.len() >= 1000 {
                                self.msgs.pop_front();
                            }
                            self.msgs.push_back(msg);
                        }
                    });

                    if log_view.button("Clear log view").clicked() {
                        self.msgs.clear();
                    }

                    if log_view.button("Close").clicked() {
                        self.show_log_view = false;
                    }
                });
        }
    }
}
