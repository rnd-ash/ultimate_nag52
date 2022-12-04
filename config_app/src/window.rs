use std::{
    borrow::BorrowMut,
    collections::VecDeque,
    ops::Add,
    time::{Duration, Instant},
};

use crate::ui::{
    main,
    status_bar::{self},
};
use eframe::{
    egui::{self, Direction, WidgetText, RichText},
    epaint::Pos2,
};
use egui_toast::{Toast, ToastKind, ToastOptions, Toasts};

pub struct MainWindow {
    pages: VecDeque<Box<dyn InterfacePage>>,
    curr_title: String,
    bar: Option<Box<dyn StatusBar>>,
    show_back: bool,
}

impl MainWindow {
    pub fn new() -> Self {
        Self {
            pages: VecDeque::new(),
            curr_title: "OpenVehicleDiag".into(),
            bar: None,
            show_back: true,
        }
    }
    pub fn add_new_page(&mut self, p: Box<dyn InterfacePage>) {
        if let Some(bar) = p.get_status_bar() {
            self.bar = Some(bar)
        }
        self.pages.push_front(p)
    }

    pub fn pop_page(&mut self) {
        self.pages.pop_front();
        if let Some(bar) = self.pages.get_mut(0).map(|x| x.get_status_bar()) {
            self.bar = bar
        }
    }
}

impl eframe::App for MainWindow {
    fn update(&mut self, ctx: &eframe::egui::Context, frame: &mut eframe::Frame) {
        let stack_size = self.pages.len();
        let mut s_bar_height = 0.0;
        if stack_size > 0 {
            let mut pop_page = false;
            if let Some(status_bar) = self.bar.borrow_mut() {
                egui::TopBottomPanel::bottom("NAV").show(ctx, |nav| {
                    nav.horizontal(|row| {
                        status_bar.draw(row, ctx);
                        if stack_size > 1 && self.show_back {
                            if row.button("Back").clicked() {
                                pop_page = true;
                            }
                        }
                    });
                    s_bar_height = nav.available_height()
                });
            }
            if pop_page {
                self.pop_page();
            }

            let mut toasts = Toasts::new()
                .anchor(Pos2::new(
                    5.0,
                    ctx.available_rect().height() - s_bar_height - 5.0,
                ))
                .align_to_end(false)
                .direction(Direction::BottomUp);

            egui::CentralPanel::default().show(ctx, |main_win_ui| {
                match self.pages[0].make_ui(main_win_ui, frame) {
                    PageAction::None => {}
                    PageAction::Destroy => self.pop_page(),
                    PageAction::Add(p) => self.add_new_page(p),
                    PageAction::Overwrite(p) => {
                        self.pages[0] = p;
                        self.bar = self.pages[0].get_status_bar();
                    }
                    PageAction::RePaint => ctx.request_repaint(),
                    PageAction::SetBackButtonState(state) => {
                        self.show_back = state;
                    }
                    PageAction::SendNotification { text, kind } => {
                        println!("Pushing notification {}", text);
                        toasts.add(Toast {
                            kind,
                            text: WidgetText::RichText(RichText::new(text)),
                            options: ToastOptions {
                                show_icon: true,
                                expires_at: Some(Instant::now().add(Duration::from_secs(5))),
                            }
                        });
                    }
                }
            });
            toasts.show(&ctx);
        }
        ctx.request_repaint();
    }
}

pub enum PageAction {
    None,
    Destroy,
    Add(Box<dyn InterfacePage>),
    Overwrite(Box<dyn InterfacePage>),
    SetBackButtonState(bool),
    RePaint,
    SendNotification { text: String, kind: ToastKind },
}

pub trait InterfacePage {
    fn make_ui(&mut self, ui: &mut egui::Ui, frame: &eframe::Frame) -> PageAction;
    fn get_title(&self) -> &'static str;
    fn get_status_bar(&self) -> Option<Box<dyn StatusBar>>;
}

pub trait StatusBar {
    fn draw(&mut self, ui: &mut egui::Ui, ctx: &egui::Context);
}
