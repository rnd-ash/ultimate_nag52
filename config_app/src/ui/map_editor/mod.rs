use std::{sync::{Mutex, Arc}, fmt::Display, borrow::BorrowMut};

use ecu_diagnostics::{kwp2000::{Kwp2000DiagnosticServer, VehicleInfo, SessionType}, DiagServerResult, DiagnosticServer, DiagError};
use eframe::{egui::{self, Layout, TextEdit, plot::{Line, HLine, LineStyle, Legend}}, epaint::Stroke};
use egui_extras::{Size, TableBuilder, Table};

use crate::window::PageAction;

use super::{status_bar::MainStatusBar, configuration::{self, cfg_structs::{TcmCoreConfig, EngineType}}};


pub const SHIFT_MAP_SIZE: usize = 44;

pub const SHIFT_MAP_X_HEADERS: [u8; 11] = [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100];
pub const UPSHIFT_MAP_Y_HEADER: [u8; 4] = [1,2,3,4];
pub const DOWNSHIFT_MAP_Y_HEADER: [u8; 4] = [2,3,4,5];

#[derive(Copy, Clone, Debug, PartialEq, Eq, PartialOrd, Ord)]
pub enum MapGroup {
    None,
    Comfort,
    Standard,
    Agility
}

pub const LARGE_FWD_RATIOS: [f32; 5] = [3.5876, 2.1862, 1.4054, 1.0000, 0.8314];
pub const SMALL_FWD_RATIOS: [f32; 5] = [3.951, 2.423, 1.486, 1.000, 0.833];

#[derive(Copy, Clone, Debug, PartialEq, Eq, PartialOrd, Ord)]
pub enum TableCalc {
    InputRpm,
    OutputRpm,
    SpeedMph,
    SpeedKmh
}

impl Display for TableCalc {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            TableCalc::InputRpm => f.write_str("Input RPM"),
            TableCalc::OutputRpm => f.write_str("Output RPM"),
            TableCalc::SpeedMph => f.write_str("Speed (Mph)"),
            TableCalc::SpeedKmh => f.write_str("Speed (Kmh)"),
        }
    }
}
impl TableCalc {
    pub fn convert_input_rpm_to_parsed(&self, raw: i16, gear: u8, cfg: &TcmCoreConfig) -> f32 {
        match self {
            TableCalc::InputRpm => raw as f32,
            TableCalc::OutputRpm => {
                let ratios = match cfg.is_large_nag() != 0 {
                    true => LARGE_FWD_RATIOS, 
                    false => SMALL_FWD_RATIOS
                };
                (raw as f32 / ratios[gear as usize-1])
            },
            TableCalc::SpeedMph => {
                let mut wheel_rpm = Self::OutputRpm.convert_input_rpm_to_parsed(raw, gear, cfg) as f32;
                wheel_rpm /= (cfg.diff_ratio() as f32) / 1000.0;
                let meters_per_hr = 60.0 * wheel_rpm * (cfg.wheel_circumference() as f32 / 1000.0);
                return (meters_per_hr * 0.000621372);
            },
            TableCalc::SpeedKmh => {
                let mut wheel_rpm = Self::OutputRpm.convert_input_rpm_to_parsed(raw, gear, cfg) as f32;
                wheel_rpm /= (cfg.diff_ratio() as f32) / 1000.0;
                let meters_per_hr = 60.0 * wheel_rpm * (cfg.wheel_circumference() as f32 / 1000.0);
                return (meters_per_hr * 0.001);
            },
        }
    }

    pub fn convert_parsed_to_raw_input(&self, parsed: i16, gear: u8, cfg: &TcmCoreConfig) -> f32 {
        match self {
            TableCalc::InputRpm => parsed as f32,
            TableCalc::OutputRpm => {
                let ratios = match cfg.is_large_nag() != 0 {
                    true => LARGE_FWD_RATIOS, 
                    false => SMALL_FWD_RATIOS
                };
                (parsed as f32 * ratios[gear as usize-1])
            },
            TableCalc::SpeedMph => {
                let meters_per_hour = parsed as f32 / 0.000621372;
                let mut wheel_rpm =  meters_per_hour / (60.0 * (cfg.wheel_circumference() as f32 / 1000.0));
                wheel_rpm *= ((cfg.diff_ratio() as f32) / 1000.0);
                return Self::OutputRpm.convert_parsed_to_raw_input(wheel_rpm as i16, gear, cfg)
            },
            TableCalc::SpeedKmh => {
                let meters_per_hour = parsed as f32 / 0.001;
                let mut wheel_rpm =  meters_per_hour / (60.0 * (cfg.wheel_circumference() as f32 / 1000.0));
                wheel_rpm *= ((cfg.diff_ratio() as f32) / 1000.0);
                return Self::OutputRpm.convert_parsed_to_raw_input(wheel_rpm as i16, gear, cfg)
            },
        }
    }
}



#[derive(Debug, Clone, Copy)]
#[repr(u8)]
#[allow(non_camel_case_types)]
pub enum MapId {
    S_DIESEL_UPSHIFT = 0x01,
    S_DIESEL_DOWNSHIFT = 0x02,
    S_PETROL_UPSHIFT = 0x03,
    S_PETROL_DOWNSHIFT = 0x04,

    C_DIESEL_UPSHIFT = 0x05,
    C_DIESEL_DOWNSHIFT = 0x06,
    C_PETROL_UPSHIFT = 0x07,
    C_PETROL_DOWNSHIFT = 0x08,

    A_DIESEL_UPSHIFT = 0x09,
    A_DIESEL_DOWNSHIFT = 0x0A,
    A_PETROL_UPSHIFT = 0x0B,
    A_PETROL_DOWNSHIFT = 0x0C,
}

pub struct MapEditor {
    bar: MainStatusBar,
    server: Arc<Mutex<Kwp2000DiagnosticServer>>,
    current_grp: MapGroup,
    upshift_map_data: Option<([i16; SHIFT_MAP_SIZE], [i16; SHIFT_MAP_SIZE])>,
    downshift_map_data: Option<([i16; SHIFT_MAP_SIZE], [i16; SHIFT_MAP_SIZE])>,
    car_config: DiagServerResult<TcmCoreConfig>,
    up_edit_text: Option<String>,
    up_edit_idx: usize,
    down_edit_text: Option<String>,
    down_edit_idx: usize,
    show_default: bool,
    display_mode : TableCalc,
    e_msg: Option<String>
}


impl MapEditor {
    pub fn new(server: Arc<Mutex<Kwp2000DiagnosticServer>>, bar: MainStatusBar) -> Self {
        let cfg = server.lock().unwrap().read_custom_local_identifier(0xFE)
            .map(|b| TcmCoreConfig::from_bytes(b.try_into().unwrap()));

        Self {
            bar, 
            server,
            current_grp: MapGroup::None,
            upshift_map_data: None,
            downshift_map_data: None,
            car_config: cfg,
            up_edit_text: None,
            up_edit_idx: 999,
            down_edit_text: None,
            down_edit_idx: 999,
            show_default: false,
            display_mode: TableCalc::InputRpm,
            e_msg: None
        }
    }

    pub fn process_map_response(data: Vec<u8>) -> DiagServerResult<[i16; SHIFT_MAP_SIZE]> {
        let size = (data[1] as u16) << 8 | data[2] as u16;
        if (size as usize != SHIFT_MAP_SIZE*2) {
            return Err(DiagError::InvalidResponseLength);
        }
        if (size as usize != data.len()-3) {
            return Err(DiagError::InvalidResponseLength);
        }

        let mut res = [0; SHIFT_MAP_SIZE];

        for x in (0..SHIFT_MAP_SIZE*2).step_by(2) {
            res[x/2] = (data[x+4] as i16) << 8 | data[x+3] as i16;
        }
        Ok(res)
    }

    fn gen_shift_table(&mut self, raw_ui: &mut egui::Ui, is_upshift: bool) -> [i16; SHIFT_MAP_SIZE] {
        let cfg = self.car_config.as_ref().unwrap();
        let (mut shift_table, shift_table_default) = match is_upshift {
            true => self.upshift_map_data.unwrap(),
            false => self.downshift_map_data.unwrap()
        };
        raw_ui.push_id(is_upshift, |ui| { 


            let mut upshift_table_builder = egui_extras::TableBuilder::new(ui)
                .striped(true)
                .scroll(false)
                .cell_layout(Layout::left_to_right(egui::Align::Center).with_cross_align(egui::Align::Center))
                .column(Size::initial(60.0).at_least(60.0));
            for _ in 0..SHIFT_MAP_X_HEADERS.len() {
                upshift_table_builder = upshift_table_builder.column(Size::initial(35.0).at_least(50.0));
            }

            upshift_table_builder.header(15.0, |mut header | {
                header.col(|u| {u.label("Pedal %");});
                for percent in SHIFT_MAP_X_HEADERS {
                    header.col(|u| {u.label(format!("{}%", percent));});
                }
            }).body(|body| {
                let mut map_idx = 0;
                body.rows(18.0, 4, |row_id, mut row| {
                    if (is_upshift) {
                        row.col(|x| { x.label(format!("{} -> {}", row_id+1, row_id+2)); });
                    } else {
                        row.col(|x| { x.label(format!("{} <- {}", row_id+1, row_id+2)); });
                    }

                    for _ in SHIFT_MAP_X_HEADERS {
                        row.col(|x| {
                            let gear = if (is_upshift) {
                                (map_idx/11) + 1
                            } else {
                                (map_idx/11) + 2
                            } as u8;

                            let edit_idx = if is_upshift { self.up_edit_idx } else { self.down_edit_idx };
                            let edit_text = if is_upshift { self.up_edit_text.borrow_mut() } else { self.down_edit_text.borrow_mut() };
                            if self.show_default {
                                x.label(format!("{}", self.display_mode.convert_input_rpm_to_parsed(shift_table_default[map_idx], gear, cfg) as i16));
                            } else {
                                let mut s = format!("{}", self.display_mode.convert_input_rpm_to_parsed(shift_table[map_idx], gear, cfg) as i16);
                                if map_idx == edit_idx {
                                    if let Some(e) = edit_text.clone() {
                                        s = e;
                                    }
                                }

                                let edit = TextEdit::singleline(&mut s);
                                let response = x.add(edit);
                                if response.gained_focus() || response.has_focus() {
                                    if edit_idx != map_idx && edit_idx != 999 {
                                        if let Some(edit) = &edit_text {
                                            if let Ok(value) = i16::from_str_radix(&edit, 10) {
                                                shift_table[edit_idx] = self.display_mode.convert_parsed_to_raw_input(value, gear, cfg) as i16;
                                            }
                                        }
                                    }
                                    if (is_upshift) {
                                        self.up_edit_text = Some(s);
                                        self.up_edit_idx = map_idx;
                                    } else {
                                        self.down_edit_text = Some(s);
                                        self.down_edit_idx = map_idx;
                                    }
                                }
                                else if response.lost_focus() {
                                    if edit_idx != 999 {
                                        if let Some(edit) = &edit_text {
                                            if let Ok(value) = i16::from_str_radix(&edit, 10) {
                                                shift_table[edit_idx] = self.display_mode.convert_parsed_to_raw_input(value, gear, cfg) as i16;
                                            }
                                        }
                                    }
                                }
                            }
                        });
                        map_idx+=1;
                    }
                });
            });
        });

        return shift_table;
    }

    pub fn gen_shift_lines(&mut self, is_default: bool, is_upshift: bool) -> Line {
        todo!()
    }

    pub fn parse_table_from_raw(&self, table: [i16; SHIFT_MAP_SIZE], display_mode: TableCalc, is_upshift: bool) -> [i16; SHIFT_MAP_SIZE] {
        let cfg = self.car_config.as_ref().unwrap();
        let mut res = [0; SHIFT_MAP_SIZE];

        for gear_idx in (0..4) {
            let gear = if (is_upshift) { gear_idx+1 } else { gear_idx+2 } as u8;
            for (pedal_idx) in (0..11) {
                res[gear_idx*11 + pedal_idx] = display_mode.convert_input_rpm_to_parsed(table[gear_idx*11 + pedal_idx], gear, cfg) as i16
            }
        }
        res
    }

    pub fn parse_table_to_raw(&self, table: [i16; SHIFT_MAP_SIZE], display_mode: TableCalc, is_upshift: bool) -> [i16; SHIFT_MAP_SIZE] {
        let cfg = self.car_config.as_ref().unwrap();
        let mut res = [0; SHIFT_MAP_SIZE];

        for gear_idx in (0..4) {
            let gear = if (is_upshift) { gear_idx+1 } else { gear_idx+2 } as u8;
            for (pedal_idx) in (0..11) {
                res[gear_idx*11 + pedal_idx] = display_mode.convert_parsed_to_raw_input(table[gear_idx*11 + pedal_idx], gear, cfg) as i16
            }
        }
        res
    }

    pub fn reset_adaptation_data(&mut self) -> DiagServerResult<()> {
        println!("Resetting adapt data");
        let mut lock = self.server.lock().unwrap();
        lock.set_diagnostic_session_mode(SessionType::ExtendedDiagnostics)?;
        lock.send_byte_array_with_response(&[0x31, 0xDD]).map(|_| ())
    }

    pub fn write_maps(&mut self) -> DiagServerResult<()> {
        let is_diesel = self.car_config.as_ref().unwrap().engine_type() == EngineType::Diesel;
        let prof = self.current_grp;
        let map_ids: [u8; 3] = match prof {
            MapGroup::None => panic!("Cannot write with no maps!"),
            MapGroup::Standard => {
                if (is_diesel) { 
                    [MapId::S_DIESEL_UPSHIFT as u8, MapId::S_DIESEL_DOWNSHIFT as u8, 0] 
                } else {
                    [MapId::S_PETROL_UPSHIFT as u8, MapId::S_PETROL_DOWNSHIFT as u8, 0]
                }
            },
            MapGroup::Comfort => {
                if (is_diesel) { 
                    [MapId::C_DIESEL_UPSHIFT as u8, MapId::C_DIESEL_DOWNSHIFT as u8, 1] 
                } else {
                    [MapId::C_PETROL_UPSHIFT as u8, MapId::C_PETROL_DOWNSHIFT as u8, 1]
                }
            },
            MapGroup::Agility => {
                if (is_diesel) { 
                    [MapId::A_DIESEL_UPSHIFT as u8, MapId::A_DIESEL_DOWNSHIFT as u8, 3] 
                } else {
                    [MapId::A_PETROL_UPSHIFT as u8, MapId::A_PETROL_DOWNSHIFT as u8, 3]
                }
            },
        };

        let mut lock = self.server.lock().unwrap();
        lock.set_diagnostic_session_mode(SessionType::ExtendedDiagnostics)?;

        let mut bytes: Vec<u8> = Vec::new();
        bytes.push(0x3B); // Write data by local ident
        bytes.push(0x19); // Map editor
        bytes.push(map_ids[0] as u8); // Upshift
        bytes.push(((SHIFT_MAP_SIZE*2) >> 8) as u8);
        bytes.push((SHIFT_MAP_SIZE*2) as u8);
        for b in self.upshift_map_data.unwrap().0 {
            bytes.push((b) as u8);
            bytes.push((b >> 8) as u8);
        }
        lock.send_byte_array_with_response(&bytes)?;
        bytes.drain(5..);
        bytes[2] = map_ids[1] as u8;
        for b in self.downshift_map_data.unwrap().0 {
            bytes.push((b) as u8);
            bytes.push((b >> 8) as u8);
        }
        lock.send_byte_array_with_response(&bytes)?;

        // Reload MAPS
        bytes.clear();
        bytes.push(0x3B); // Write data by local ident
        bytes.push(0x19); // Map editor
        bytes.push(0xFF); // RELOAD
        bytes.push(map_ids[2]); // Reload profileID
        lock.send_byte_array_with_response(&bytes)?;

        Ok(())
    }

    pub fn read_maps(&mut self) {
        let is_diesel = self.car_config.as_ref().unwrap().engine_type() == EngineType::Diesel;
        let prof = self.current_grp;
        let map_ids: [u8; 2] = match prof {
            MapGroup::None => panic!("Cannot query with no maps!"),
            MapGroup::Standard => {
                if (is_diesel) { 
                    [MapId::S_DIESEL_UPSHIFT as u8, MapId::S_DIESEL_DOWNSHIFT as u8] 
                } else {
                    [MapId::S_PETROL_UPSHIFT as u8, MapId::S_PETROL_DOWNSHIFT as u8]
                }
            },
            MapGroup::Comfort => {
                if (is_diesel) { 
                    [MapId::C_DIESEL_UPSHIFT as u8, MapId::C_DIESEL_DOWNSHIFT as u8] 
                } else {
                    [MapId::C_PETROL_UPSHIFT as u8, MapId::C_PETROL_DOWNSHIFT as u8]
                }
            },
            MapGroup::Agility => {
                if (is_diesel) { 
                    [MapId::A_DIESEL_UPSHIFT as u8, MapId::A_DIESEL_DOWNSHIFT as u8] 
                } else {
                    [MapId::A_PETROL_UPSHIFT as u8, MapId::A_PETROL_DOWNSHIFT as u8]
                }
            },
        };

        let mut lock = self.server.lock().unwrap();
        println!("Querying maps in {:?}", prof);

        let upshift = lock.send_byte_array_with_response(&[0x21, 0x19, map_ids[0]]).and_then(|x| Self::process_map_response(x));
        let upshift_default = lock.send_byte_array_with_response(&[0x21, 0x19, map_ids[0] | 0x80]).and_then(|x| Self::process_map_response(x));

        let downshift = lock.send_byte_array_with_response(&[0x21, 0x19, map_ids[1]]).and_then(|x| Self::process_map_response(x));
        let downshift_default = lock.send_byte_array_with_response(&[0x21, 0x19, map_ids[1] | 0x80]).and_then(|x| Self::process_map_response(x));

        if upshift.is_ok() && upshift_default.is_ok() && downshift.is_ok() && downshift_default.is_ok() {
            self.upshift_map_data = Some((upshift.unwrap(), upshift_default.unwrap()));
            self.downshift_map_data = Some((downshift.unwrap(), downshift_default.unwrap()));

        } else {
            self.downshift_map_data = None;
            self.upshift_map_data = None;
        }

    }
}


impl super::InterfacePage for MapEditor {
    fn make_ui(&mut self, ui: &mut eframe::egui::Ui, frame: &eframe::Frame) -> crate::window::PageAction {
        if let Err(e) = &self.car_config {
            ui.label(
                format!("
                A fatal error has occurred trying to load the map editor

                Could not read vehicle information from the TCU:

                Diagnostic error:

                {}

                Press try again to attempt to query the ECU again
                ", e)
            );
            if ui.button("Try to query again").clicked() {
                self.car_config = self.server.lock().unwrap().read_custom_local_identifier(0xFE)
                .map(|b| TcmCoreConfig::from_bytes(b.try_into().unwrap()));
            }
            return PageAction::None;
        }


        let mut current_group = self.current_grp;
        egui::menu::bar(ui, |bar| {
            bar.label("Select profile: ");
            if bar.selectable_label(current_group == MapGroup::Standard, "Standard (S)").clicked() {
                current_group = MapGroup::Standard;
            }
            if bar.selectable_label(current_group == MapGroup::Comfort, "Comfort (C)").clicked() {
                current_group = MapGroup::Comfort;
            }
            if bar.selectable_label(current_group == MapGroup::Agility, "Agility (A)").clicked() {
                current_group = MapGroup::Agility;
            }
            if bar.button("Reset adaptation data").clicked() {
                match self.reset_adaptation_data() {
                    Ok(_) => {
                        self.e_msg = Some(format!("Adaptation reset OK!"));
                    },
                    Err(e) => {
                        self.e_msg = Some(format!("Error resetting adaptation data: {}", e));
                    }
                }
            }
        });

        if self.current_grp != current_group {
            // Query ECU
            self.current_grp = current_group;
            self.read_maps();
        }

        if self.current_grp == MapGroup::None {
            return PageAction::None;
        }

        if self.upshift_map_data.is_none() || self.downshift_map_data.is_none() {
            ui.label(
                "
                An error occurred trying to read map data from the ECU. Please try again.
                "
            );
            if ui.button("Try to query maps again").clicked() {
                self.read_maps();
            }
        } else {
            // SHOW THE UI!
            ui.checkbox(&mut self.show_default, "Show default maps");


            let mut curr_display_mode = self.display_mode;

            egui::ComboBox::new("DispMode", "Value representation")
                .selected_text(format!("{}", self.display_mode))
                .show_ui(ui, |menu| {
                menu.selectable_value(&mut curr_display_mode, TableCalc::InputRpm, format!("{}", TableCalc::InputRpm));
                menu.selectable_value(&mut curr_display_mode, TableCalc::OutputRpm, format!("{}", TableCalc::OutputRpm));
                // TODO Handle 4Matic configs
                if self.car_config.as_ref().unwrap().is_four_matic() == 0 {
                    menu.selectable_value(&mut curr_display_mode, TableCalc::SpeedKmh, format!("{}", TableCalc::SpeedKmh));
                    menu.selectable_value(&mut curr_display_mode, TableCalc::SpeedMph, format!("{}", TableCalc::SpeedMph));
                }
            });
            if (self.display_mode != curr_display_mode) {
                self.up_edit_idx = 999;
                self.up_edit_text = None;
                self.down_edit_idx = 999;
                self.down_edit_text = None;
                self.display_mode = curr_display_mode;
            }


            ui.heading("Upshift table");
            self.upshift_map_data.as_mut().unwrap().0 = self.gen_shift_table(ui, true);

            ui.heading("Downshift table");
            self.downshift_map_data.as_mut().unwrap().0 = self.gen_shift_table(ui, false);

            let redline = if self.car_config.as_ref().unwrap().engine_type() == EngineType::Diesel {
                self.car_config.as_ref().unwrap().red_line_dieselrpm()
            } else {
                self.car_config.as_ref().unwrap().red_line_petrolrpm()
            } as i16;


            ui.horizontal(|row| {
                if row.button("Write maps to ECU").clicked() {
                    self.e_msg = None;
                    if let Err(e) = self.write_maps() {
                        self.e_msg = Some(format!("Error writing maps: {}", e));
                    } else {
                        self.e_msg = Some(format!("Map write OK!"));
                    }
                    self.read_maps();
                }
                if row.button("Reset to default (W/O write to ECU)").clicked() {
                    if let Some(us) = self.upshift_map_data.borrow_mut() {
                        us.0 = us.1;
                    }
                    if let Some(us) = self.downshift_map_data.borrow_mut() {
                        us.0 = us.1;
                    }
                }
            });



            let mut lines: Vec<Line> = Vec::new();

            let cfg = self.car_config.as_ref().unwrap();
            let upshift_data = self.upshift_map_data.map(|(x, d) | {
                if self.show_default { d } else {x}
            }).unwrap();
            
            let downshift_data = self.downshift_map_data.map(|(x, d) | {
                if self.show_default { d } else {x}
            }).unwrap();

            for x in (0..4) {
                let mut points: Vec<[f64; 2]> = Vec::new();
                for ped in (0..=10).step_by(1) {
                    points.push([10.0*ped as f64, self.display_mode.convert_input_rpm_to_parsed(upshift_data[x*11 + ped], (x+1) as u8, cfg) as f64]);
                }
                lines.push(Line::new(points).name(format!("Upshift {}-{}", x+1, x+2)))
            }

            for x in (0..4) {
                let mut points: Vec<[f64; 2]> = Vec::new();
                for ped in (0..=10).step_by(1) {
                    points.push([10.0*ped as f64, self.display_mode.convert_input_rpm_to_parsed(downshift_data[x*11 + ped], (x+1) as u8, cfg) as f64]);
                }
                lines.push(Line::new(points).style(LineStyle::Dashed { length: 5.0 }).name(format!("Downshift {}-{}", x+2, x+1)))
            }


            egui::plot::Plot::new("Shift zones")
                .include_x(0)
                .include_y(0)
                .include_x(100)
                .legend(Legend::default())
                .show(ui, |plot_ui| {
                    plot_ui.hline(HLine::new(self.display_mode.convert_input_rpm_to_parsed(redline, 5, &self.car_config.as_ref().unwrap())));
                    for shift_line in lines {
                        plot_ui.line(shift_line);
                    }
                });           
        }
        if let Some(msg) = &self.e_msg {
            ui.label(msg);
        } 
        crate::window::PageAction::None
    }

    fn get_title(&self) -> &'static str {
        "Map editor"
    }

    fn get_status_bar(&self) -> Option<Box<dyn crate::window::StatusBar>> {
        Some(Box::new(self.bar.clone()))
    }
}