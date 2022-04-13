use ecu_diagnostics::hardware::Hardware;
use egui::*;
use epi::*;
use std::{
    collections::VecDeque,
    sync::{Arc, Mutex, RwLock}, borrow::BorrowMut,
};

use crate::{
    usb_hw::diag_usb::{EspLogMessage, Nag52USB},
    window::{InterfacePage, StatusBar},
};

#[derive(Clone)]
pub struct MainStatusBar {
    dev: Arc<Mutex<Nag52USB>>,
    show_log_view: bool,
    msgs: VecDeque<EspLogMessage>,
    auto_scroll: bool,
    use_light_theme: bool
}

impl MainStatusBar {
    pub fn new(dev: Arc<Mutex<Nag52USB>>) -> Self {
        Self {
            dev,
            show_log_view: false,
            msgs: VecDeque::new(),
            auto_scroll: true,
            use_light_theme: false
        }
    }
}

impl StatusBar for MainStatusBar {
    fn draw(&mut self, ui: &mut egui::Ui, ctx: &egui::Context) {
        match self.dev.lock().unwrap().is_connected() {
            true => ui.label("Connected"),
            false => ui.label("Disconnected"),
        };
        if ui.button("TCM log view").clicked() {
            self.show_log_view = true;
        }

        if ui.checkbox(&mut self.use_light_theme, "Light theme").clicked() {
            let style = match self.use_light_theme {
                true => egui::Visuals::light(),
                false => egui::Visuals::dark()
            };
            ctx.set_visuals(style);

        }


        if self.show_log_view {
            egui::containers::Window::new("Log view")
                .fixed_size(&[1200f32, 400f32])
                .resizable(false)
                .show(ui.ctx(), |log_view| {
                    log_view.vertical(|l_view| {
                        egui::containers::ScrollArea::new([false, true])
                            .max_height(300f32)
                            .auto_shrink([false, false])
                            .stick_to_bottom()
                            .show(l_view, |scroll| {
                                for msg in &self.msgs {
                                    scroll.label(
                                        RichText::new(format!(
                                            "{} {} {}",
                                            msg.timestamp, msg.tag, msg.msg
                                        ))
                                        .color(match msg
                                            .lvl
                                        {
                                            crate::usb_hw::diag_usb::EspLogLevel::Info => {
                                                Color32::from_rgb(0, 100, 0)
                                            }
                                            crate::usb_hw::diag_usb::EspLogLevel::Warn => {
                                                Color32::from_rgb(255, 215, 0)
                                            }
                                            crate::usb_hw::diag_usb::EspLogLevel::Error => {
                                                Color32::from_rgb(255, 0, 0)
                                            }
                                            _ => Color32::from_rgb(255, 255, 255),
                                        }),
                                    );
                                }
                            });
                        while let Some(msg) = self.dev.lock().unwrap().read_log_msg() {
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
