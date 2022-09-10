use ecu_diagnostics::hardware::Hardware;
use eframe::egui::*;
use std::{
    collections::VecDeque,
    sync::{Arc, Mutex, RwLock, mpsc}, borrow::BorrowMut,
};

use crate::{
    usb_hw::diag_usb::{EspLogMessage, Nag52USB},
    window::{InterfacePage, StatusBar},
};
use eframe::egui;

#[derive(Clone)]
pub struct MainStatusBar {
    show_log_view: bool,
    msgs: VecDeque<EspLogMessage>,
    receiver: Option<Arc<mpsc::Receiver<EspLogMessage>>>,
    hw_name: String,
    auto_scroll: bool,
    use_light_theme: bool
}

impl MainStatusBar {
    pub fn new(logger: Option<mpsc::Receiver<EspLogMessage>>, hw_name: String) -> Self {
        Self {
            show_log_view: false,
            msgs: VecDeque::new(),
            auto_scroll: true,
            use_light_theme: false,
            receiver: match logger {
                Some(l) => Some(Arc::new(l)),
                None => None,
            },
            hw_name
        }
    }
}

impl StatusBar for MainStatusBar {
    fn draw(&mut self, ui: &mut egui::Ui, ctx: &egui::Context) {
        ui.label(format!("Connected via {}", self.hw_name));
        if self.receiver.is_some() {
            if ui.button("TCM log view").clicked() {
                self.show_log_view = true;
            }
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
                            .stick_to_bottom(true)
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
                        //while let Some(msg) = self.dev.lock().unwrap().read_log_msg() {
                        //    if self.msgs.len() >= 1000 {
                        //        self.msgs.pop_front();
                        //    }
                        //    self.msgs.push_back(msg);
                        //}
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
