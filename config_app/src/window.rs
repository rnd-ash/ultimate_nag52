use std::{borrow::BorrowMut, collections::VecDeque};

use ecu_diagnostics::hardware::Hardware;
use eframe::{
    egui,
    epi::{self, Storage},
};

use crate::ui::status_bar::{self};

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

impl epi::App for MainWindow {
    fn update(&mut self, ctx: &egui::CtxRef, frame: &epi::Frame) {
        let stack_size = self.pages.len();
        if stack_size > 0 {
            let mut pop_page = false;
            if let Some(status_bar) = self.bar.borrow_mut() {
                egui::TopBottomPanel::bottom("NAV")
                    .default_height(800.0)
                    .show(ctx, |nav| {
                        nav.horizontal(|row| {
                            status_bar.draw(row);
                            if stack_size > 1 && self.show_back {
                                if row.button("Back").clicked() {
                                    pop_page = true;
                                }
                            }
                        });
                    });
            }
            if pop_page {
                self.pop_page();
            }

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
                }
            });
        }
        ctx.request_repaint(); // Continuous mode
    }

    fn name(&self) -> &str {
        if self.pages.len() > 0 {
            self.pages[0].get_title()
        } else {
            "Ultimate-Nag52 configuration utility"
        }
    }
}

pub enum PageAction {
    None,
    Destroy,
    Add(Box<dyn InterfacePage>),
    Overwrite(Box<dyn InterfacePage>),
    SetBackButtonState(bool),
    RePaint,
}

pub trait InterfacePage {
    fn make_ui(&mut self, ui: &mut egui::Ui, frame: &epi::Frame) -> PageAction;
    fn get_title(&self) -> &'static str;
    fn get_status_bar(&self) -> Option<Box<dyn StatusBar>>;
}

pub trait StatusBar {
    fn draw(&mut self, ui: &mut egui::Ui);
}
