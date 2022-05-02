use std::{sync::{Arc, Mutex}, borrow::BorrowMut};

use ecu_diagnostics::{hardware::Hardware, kwp2000::{Kwp2000DiagnosticServer, Kwp2000ServerOptions, Kwp2000VoidHandler, SessionType, ResetMode}, DiagnosticServer};
use egui::{Ui, Label};
use epi::Frame;
use crate::{usb_hw::diag_usb::Nag52USB, window::PageAction};

use self::cfg_structs::{TcmCoreConfig, DefaultProfile, EngineType};

use super::{status_bar::MainStatusBar, StatusText};

pub mod cfg_structs;

pub struct ConfigPage {
    server: Arc<Mutex<Kwp2000DiagnosticServer>>,
    bar: MainStatusBar,
    status: StatusText,
    scn: Option<TcmCoreConfig>
}

impl ConfigPage {
    pub fn new(server: Arc<Mutex<Kwp2000DiagnosticServer>>, bar: MainStatusBar) -> Self {
        Self {
            server,
            bar,
            status: StatusText::Ok("".into()),
            scn: None
        }
    }
}


impl crate::window::InterfacePage for ConfigPage {
    fn make_ui(&mut self, ui: &mut Ui, frame: &Frame) -> PageAction {
        ui.heading("TCM Configuration");


        if ui.button("Read Configuration").clicked() {
            match self.server.lock().unwrap().read_custom_local_identifier(0xFE) {
                Ok(res) => {
                    self.scn = Some(TcmCoreConfig::from_bytes(res.try_into().unwrap()));
                    self.status = StatusText::Ok(format!("Read OK!"));
                },
                Err(e) => {
                    self.status = StatusText::Err(format!("Error reading configuration: {}", e))
                }
            }
        }

        if let Some(scn) = self.scn.borrow_mut() {

            egui::Grid::new("DGS").striped(true).show(ui, |ui| {
                let mut x = scn.is_large_nag() == 1;
                ui.label("Using large 722.6");
                ui.checkbox(&mut x, "");
                scn.set_is_large_nag(x as u8);
                ui.end_row();


                let mut curr_profile = scn.default_profile();
                ui.label("Default drive profile");
                egui::ComboBox::from_id_source("profile")
                .width(100.0)
                .selected_text(format!("{:?}", curr_profile))
                .show_ui(ui, |cb_ui| {
                    let profiles = vec![
                        DefaultProfile::Standard,
                        DefaultProfile::Comfort,
                        DefaultProfile::Winter,
                        DefaultProfile::Agility,
                        DefaultProfile::Manual
                    ];
                    for dev in profiles {
                        cb_ui.selectable_value(&mut curr_profile, dev.clone(), format!("{:?}", dev));
                    }
                    scn.set_default_profile(curr_profile)
                });
                ui.end_row();


                let mut buffer = format!("{:.2}", scn.diff_ratio() as f32 / 1000.0);
                ui.label("Differential ratio");
                ui.text_edit_singleline(&mut buffer);
                if let Ok(new_ratio) = buffer.parse::<f32>() {
                    scn.set_diff_ratio((new_ratio * 1000.0) as u16);
                }
                ui.end_row();

                let mut buffer = format!("{}", scn.wheel_circumference());
                ui.label("Wheel circumferance (mm)");
                ui.text_edit_singleline(&mut buffer);
                if let Ok(new_ratio) = buffer.parse::<u16>() {
                    scn.set_wheel_circumference(new_ratio);
                }
                ui.end_row();


                let mut engine = scn.engine_type();
                ui.label("Engine type");
                egui::ComboBox::from_id_source("engine_type")
                .width(100.0)
                .selected_text(format!("{:?}", engine))
                .show_ui(ui, |cb_ui| {
                    let profiles = vec![
                        EngineType::Diesel,
                        EngineType::Petrol
                    ];
                    for dev in profiles {
                        cb_ui.selectable_value(&mut engine, dev.clone(), format!("{:?}", dev));
                    }
                    scn.set_engine_type(engine)
                });
                ui.end_row();


                let mut buffer = match scn.engine_type() {
                    EngineType::Diesel => format!("{}", scn.red_line_dieselrpm()),
                    EngineType::Petrol => format!("{}", scn.red_line_petrolrpm())
                };
                ui.label("Engine redline RPM");
                ui.text_edit_singleline(&mut buffer);
                if let Ok(rpm) = buffer.parse::<u16>() {
                    match scn.engine_type() {
                        EngineType::Diesel => scn.set_red_line_dieselrpm(rpm),
                        EngineType::Petrol => scn.set_red_line_petrolrpm(rpm)
                    }
                }
                ui.end_row();

                let mut x = scn.is_four_matic() == 1;
                ui.label("Four matic");
                ui.checkbox(&mut x, "");
                scn.set_is_four_matic(x as u8);
                ui.end_row();

                if scn.is_four_matic() == 1 {

                    let mut buffer = format!("{:.2}", scn.transfer_case_high_ratio() as f32 / 1000.0);
                    ui.label("Transfer case high ratio");
                    ui.text_edit_singleline(&mut buffer);
                    if let Ok(new_ratio) = buffer.parse::<f32>() {
                        scn.set_transfer_case_high_ratio((new_ratio * 1000.0) as u16);
                    }
                    ui.end_row();

                    let mut buffer = format!("{:.2}", scn.transfer_case_low_ratio() as f32 / 1000.0);
                    ui.label("Transfer case low ratio");
                    ui.text_edit_singleline(&mut buffer);
                    if let Ok(new_ratio) = buffer.parse::<f32>() {
                        scn.set_transfer_case_low_ratio((new_ratio * 1000.0) as u16);
                    }
                    ui.end_row();
                }

            });






            if ui.button("Write configuration").clicked() {
                let res = {
                    let mut x: Vec<u8> = vec![0x3B, 0xFE];
                    x.extend_from_slice(&scn.clone().into_bytes());
                    self.server.lock().unwrap().set_diagnostic_session_mode(SessionType::ExtendedDiagnostics);
                    self.server.lock().unwrap().send_byte_array_with_response(&x);
                    self.server.lock().unwrap().reset_ecu(ResetMode::PowerOnReset);
                };
            }
        }

        ui.add(self.status.clone());
        PageAction::None
    }

    fn get_title(&self) -> &'static str {
        "Configuration"
    }

    fn get_status_bar(&self) -> Option<Box<dyn crate::window::StatusBar>> {
        Some(Box::new(self.bar.clone()))
    }
}