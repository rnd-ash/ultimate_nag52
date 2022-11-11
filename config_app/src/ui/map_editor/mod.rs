use std::{
    borrow::BorrowMut,
    collections::HashMap,
    fmt::Display,
    sync::{Arc, Mutex},
};

use ecu_diagnostics::{
    kwp2000::{
        self, KWP2000Command, Kwp2000Cmd, Kwp2000DiagnosticServer, SessionType, VehicleInfo,
    },
    DiagError, DiagServerResult, DiagnosticServer,
};
use eframe::{
    egui::{
        self,
        plot::{Bar, BarChart, CoordinatesFormatter, HLine, Legend, Line, LineStyle, PlotPoints},
        Layout, TextEdit, RichText,
    },
    epaint::{vec2, Color32, Stroke, FontId, TextShape},
};
use egui_extras::{Size, Table, TableBuilder};
use egui_toast::ToastKind;
use nom::number::complete::le_u16;
mod map_widget;

use crate::window::PageAction;

use self::map_widget::MapWidget;

use super::{
    configuration::{
        self,
        cfg_structs::{EngineType, TcmCoreConfig},
    },
    status_bar::MainStatusBar,
};

#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum MapCmd {
    Read = 0x01,
    ReadDefault = 0x02,
    Write = 0x03,
    Burn = 0x04,
    ResetToFlash = 0x05,
    Undo = 0x06,
    ReadMeta = 0x07,
}

#[derive(Debug, Clone)]
pub struct Map {
    meta: MapData,
    x_values: Vec<i16>,
    y_values: Vec<i16>,
    eeprom_key: String,
    data_eeprom: Vec<i16>,
    data_default: Vec<i16>,
    data_modify: Vec<i16>,
    showing_default: bool,
    ecu_ref: Arc<Mutex<Kwp2000DiagnosticServer>>,
}

fn read_i16(a: &[u8]) -> DiagServerResult<(&[u8], i16)> {
    if a.len() < 2 {
        return Err(DiagError::InvalidResponseLength);
    }
    let r = i16::from_le_bytes(a[0..2].try_into().unwrap());
    Ok((&a[2..], r))
}

fn read_u16(a: &[u8]) -> DiagServerResult<(&[u8], u16)> {
    if a.len() < 2 {
        return Err(DiagError::InvalidResponseLength);
    }
    let r = u16::from_le_bytes(a[0..2].try_into().unwrap());
    Ok((&a[2..], r))
}

impl Map {
    pub fn new(
        map_id: u8,
        server: Arc<Mutex<Kwp2000DiagnosticServer>>,
        meta: MapData,
    ) -> DiagServerResult<Self> {
        // Read metadata
        let mut s = server.lock().unwrap();
        let mut ecu_response = &s.send_byte_array_with_response(&[
            KWP2000Command::ReadDataByLocalIdentifier.into(),
            0x19,
            map_id,
            MapCmd::ReadMeta as u8,
            0x00,
            0x00,
        ])?[1..];
        let (data, data_len) = read_u16(ecu_response)?;
        if data.len() != data_len as usize {
            return Err(DiagError::InvalidResponseLength);
        }
        let (data, x_element_count) = read_u16(data)?;
        let (data, y_element_count) = read_u16(data)?;
        let (mut data, key_len) = read_u16(data)?;
        if data.len() as u16 != ((x_element_count + y_element_count) * 2) + key_len {
            return Err(DiagError::InvalidResponseLength);
        }
        let mut x_elements: Vec<i16> = Vec::new();
        let mut y_elements: Vec<i16> = Vec::new();
        for _ in 0..x_element_count {
            let (d, v) = read_i16(data)?;
            x_elements.push(v);
            data = d;
        }
        for _ in 0..y_element_count {
            let (d, v) = read_i16(data)?;
            y_elements.push(v);
            data = d;
        }
        let key = String::from_utf8(data.to_vec()).unwrap();

        let mut default: Vec<i16> = Vec::new();
        let mut current: Vec<i16> = Vec::new();

        // Read current data
        let ecu_response = &s.send_byte_array_with_response(&[
            KWP2000Command::ReadDataByLocalIdentifier.into(),
            0x19,
            map_id,
            MapCmd::Read as u8,
            0x00,
            0x00,
        ])?[1..];
        let (mut c_data, c_arr_size) = read_u16(ecu_response)?;
        if c_data.len() != c_arr_size as usize {
            return Err(DiagError::InvalidResponseLength);
        }
        for _ in 0..(c_arr_size / 2) {
            let (d, v) = read_i16(c_data)?;
            current.push(v);
            c_data = d;
        }

        // Read default data
        let ecu_response = &s.send_byte_array_with_response(&[
            KWP2000Command::ReadDataByLocalIdentifier.into(),
            0x19,
            map_id,
            MapCmd::ReadDefault as u8,
            0x00,
            0x00,
        ])?[1..];
        drop(s); // Drop Mutex lock
        let (mut d_data, d_arr_size) = read_u16(ecu_response)?;
        if d_data.len() != d_arr_size as usize {
            return Err(DiagError::InvalidResponseLength);
        }
        for _ in 0..(d_arr_size / 2) {
            let (d, v) = read_i16(d_data)?;
            default.push(v);
            d_data = d;
        }
        println!(
            "({}x{}) Default len {}, current len {}",
            x_element_count,
            y_element_count,
            default.len(),
            current.len()
        );
        Ok(Self {
            x_values: x_elements,
            y_values: y_elements,
            eeprom_key: key,
            data_eeprom: current.clone(),
            data_default: default,
            data_modify: current,
            meta,
            showing_default: false,
            ecu_ref: server,
        })
    }

    fn get_x_label(&self, idx: usize) -> String {
        if let Some(replace) = self.meta.x_replace {
            format!("{}", replace.get(idx).unwrap_or(&"ERROR"))
        } else {
            format!("{} {}", self.x_values[idx], self.meta.x_unit)
        }
    }

    fn get_y_label(&self, idx: usize) -> String {
        if let Some(replace) = self.meta.y_replace {
            format!("{}", replace.get(idx).unwrap_or(&"ERROR"))
        } else {
            format!("{} {}", self.y_values[idx], self.meta.y_unit)
        }
    }

    fn gen_edit_table(&mut self, raw_ui: &mut egui::Ui, showing_default: bool) {
        let hash = if showing_default { &self.data_default } else { &self.data_eeprom };
        let color = raw_ui.visuals().faint_bg_color;
        
        let resp = raw_ui.push_id(&hash, |ui| {
            let mut table_builder = egui_extras::TableBuilder::new(ui)
                .striped(true)
                .scroll(false)
                .cell_layout(Layout::left_to_right(egui::Align::Center).with_cross_align(egui::Align::Center))
                .column(Size::initial(60.0).at_least(60.0));
            for _ in 0..self.x_values.len() {
                table_builder = table_builder.column(Size::initial(60.0).at_least(70.0));
            }
            table_builder.header(15.0, |mut header | {
                header.col(|u| {}); // Nothing in corner cell
                for v in &self.x_values {
                    header.col(|u| {
                        u.label(RichText::new(format!("{}", v)).color(color));
                    });
                }
            }).body(|body| {
                body.rows(18.0, self.y_values.len(), |row_id, mut row| {
                    if let Some(replace) = self.meta.y_replace {
                        row.col(|x| { x.label(format!("{}", replace.get(row_id).unwrap_or(&"ERROR"))); });
                    } else {
                        row.col(|x| { x.label(format!("{}{}", self.y_values[row_id], self.meta.y_unit)); });
                    }
                    for x_pos in 0..self.x_values.len() {
                        row.col(|cell| {

                            let value = match showing_default {
                                true => self.data_default.get((row_id*self.x_values.len())+x_pos).unwrap_or(&30000),
                                false => self.data_eeprom.get((row_id*self.x_values.len())+x_pos).unwrap_or(&30000)
                            };
                                // Add X values
                            cell.label(format!("{}", value));
                        });
                    }
                })
            });
        });

        let line_visuals = raw_ui.visuals().text_color();
        let size = resp.response.rect;
       
        //raw_ui.painter().add(TextShape {
        //    pos: (size.left()-galley_y.size().x, size.bottom()).into(),
        //    galley: galley_y,
        //    underline: Stroke::none(),
        //    override_text_color: None,
        //    angle: -1.5708,
        //});


    }

    fn generate_window_ui(&mut self, raw_ui: &mut egui::Ui) {
        raw_ui.label(format!("EEPROM key: {}", self.eeprom_key));
        raw_ui.label(format!(
            "Map has {} elements",
            self.x_values.len() * self.y_values.len()
        ));
        self.gen_edit_table(raw_ui, self.showing_default);
        // Generate display chart
        if self.x_values.len() == 1 {
            // Bar chart
            let mut bars = Vec::new();
            for x in 0..self.y_values.len() {
                // Distinct points
                let value = if self.showing_default {
                    self.data_default[x]
                } else {
                    self.data_modify[x]
                };
                let key = self.get_y_label(x);
                bars.push(Bar::new(x as f64, value as f64).name(key))
            }
            egui::plot::Plot::new(format!("PLOT-{}", self.eeprom_key))
                .allow_drag(false)
                .allow_scroll(false)
                .allow_zoom(false)
                .height(150.0)
                .include_x(0)
                .include_y((self.y_values.len() + 1) as f64 * 1.5)
                .show(raw_ui, |plot_ui| plot_ui.bar_chart(BarChart::new(bars)));
        } else { // Line chart
            let mut lines: Vec<Line> = Vec::new();
            for (y_idx, key) in self.y_values.iter().enumerate() {
                let mut points : Vec<[f64;2]> = Vec::new();
                for (x_idx, key) in self.x_values.iter().enumerate() {
                    let data = self.data_modify[(y_idx*self.x_values.len())+x_idx];
                    points.push([*key as f64, data as f64]);
                }
                lines.push(
                    Line::new(points).name(self.get_y_label(y_idx))
                );
            }
            egui::plot::Plot::new(format!("PLOT-{}", self.eeprom_key))
                .allow_drag(false)
                .allow_scroll(false)
                .allow_zoom(false)
                .height(150.0)
                .show(raw_ui, |plot_ui| {
                    for l in lines {
                        plot_ui.line(l);
                    }
                });

        }
        raw_ui.checkbox(&mut self.showing_default, "Show flash defaults");
        if self.data_modify != self.data_eeprom {
            if raw_ui.button("Undo user changes").clicked() {}
            if raw_ui.button("Write changes (To RAM)").clicked() {}
            if raw_ui.button("Write changes (To EEPROM)").clicked() {}
        }
        if self.data_modify != self.data_default {
            if raw_ui.button("Reset to flash defaults").clicked() {}
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub struct MapData {
    id: u8,
    name: &'static str,
    x_unit: &'static str,
    y_unit: &'static str,
    x_desc: &'static str,
    y_desc: &'static str,
    value_unit: &'static str,
    x_replace: Option<&'static [&'static str]>,
    y_replace: Option<&'static [&'static str]>,
}

impl MapData {
    pub const fn new(
        id: u8,
        name: &'static str,
        x_unit: &'static str,
        y_unit: &'static str,
        x_desc: &'static str,
        y_desc: &'static str,
        value_unit: &'static str,
        x_replace: Option<&'static [&'static str]>,
        y_replace: Option<&'static [&'static str]>,
    ) -> Self {
        Self {
            id,
            name,
            x_unit,
            y_unit,
            x_desc,
            y_desc,
            value_unit,
            x_replace,
            y_replace,
        }
    }
}

const MAP_ARRAY: [MapData; 11] = [
    MapData::new(
        0x01,
        "Upshift (A)",
        "%",
        "",
        "Pedal position (%)",
        "Gear shift",
        "RPM",
        None,
        Some(&["1->2", "2->3", "3->4", "4->5"]),
    ),
    MapData::new(
        0x02,
        "Upshift (C)",
        "%",
        "",
        "Pedal position (%)",
        "Gear shift",
        "RPM",
        None,
        Some(&["1->2", "2->3", "3->4", "4->5"]),
    ),
    MapData::new(
        0x03,
        "Upshift (S)",
        "%",
        "",
        "Pedal position (%)",
        "Gear shift",
        "RPM",
        None,
        Some(&["1->2", "2->3", "3->4", "4->5"]),
    ),
    MapData::new(
        0x04,
        "Downshift (A)",
        "%",
        "",
        "Pedal position (%)",
        "Gear shift",
        "RPM",
        None,
        Some(&["2->1", "3->2", "4->3", "5->4"]),
    ),
    MapData::new(
        0x05,
        "Downshift (C)",
        "%",
        "",
        "Pedal position (%)",
        "Gear shift",
        "RPM",
        None,
        Some(&["2->1", "3->2", "4->3", "5->4"]),
    ),
    MapData::new(
        0x06,
        "Downshift (S)",
        "%",
        "",
        "Pedal position (%)",
        "Gear shift",
        "RPM",
        None,
        Some(&["2->1", "3->2", "4->3", "5->4"]),
    ),
    MapData::new(
        0x07,
        "Working pressure",
        "%",
        "",
        "Input torque (% of rated)",
        "Gear",
        "mBar",
        None,
        Some(&["P/N", "R1/R2", "1", "2", "3", "4", "5"]),
    ),
    MapData::new(
        0x08,
        "Pressure solenoid current",
        "mBar",
        "C",
        "Working pressure",
        "ATF Temperature",
        "mA",
        None,
        None,
    ),
    MapData::new(
        0x09,
        "TCC solenoid Pwm",
        "mBar",
        "C",
        "Converter pressure",
        "ATF Temperature",
        "/4096",
        None,
        None,
    ),
    MapData::new(
        0x0A,
        "Clutch filling time",
        "C",
        "",
        "",
        "Clutch",
        "ms",
        None,
        Some(&["K1", "K2", "K3", "B1", "B2"]),
    ),
    MapData::new(
        0x0B,
        "Clutch filling pressure",
        "C",
        "",
        "",
        "Clutch",
        "mBar",
        None,
        Some(&["K1", "K2", "K3", "B1", "B2"]),
    ),
];

pub struct MapEditor {
    bar: MainStatusBar,
    server: Arc<Mutex<Kwp2000DiagnosticServer>>,
    loaded_maps: HashMap<String, Map>,
    error: Option<String>,
}

impl MapEditor {
    pub fn new(server: Arc<Mutex<Kwp2000DiagnosticServer>>, bar: MainStatusBar) -> Self {
        Self {
            bar,
            server,
            loaded_maps: HashMap::new(),
            error: None,
        }
    }
}

impl super::InterfacePage for MapEditor {
    fn make_ui(
        &mut self,
        ui: &mut eframe::egui::Ui,
        frame: &eframe::Frame,
    ) -> crate::window::PageAction {
        for map in &MAP_ARRAY {
            if ui.button(map.name).clicked() {
                self.error = None;
                match Map::new(map.id, self.server.clone(), map.clone()) {
                    Ok(m) => {
                        // Only if map is not already loaded
                        if !self.loaded_maps.contains_key(&m.eeprom_key) {
                            self.loaded_maps.insert(m.eeprom_key.clone(), m);
                        }
                    }
                    Err(e) => self.error = Some(e.to_string()),
                }
            }
        }

        let mut remove_list: Vec<String> = Vec::new();
        for (key, map) in self.loaded_maps.iter_mut() {
            let mut open = true;
            egui::Window::new(map.meta.name)
                .auto_sized()
                .collapsible(true)
                .open(&mut open)
                .vscroll(false)
                .default_size(vec2(800.0, 400.0))
                .show(ui.ctx(), |window| {
                    map.generate_window_ui(window);
                });
            if !open {
                remove_list.push(key.clone())
            }
        }
        for key in remove_list {
            self.loaded_maps.remove(&key);
        }
        PageAction::None
    }

    fn get_title(&self) -> &'static str {
        "Map editor"
    }

    fn get_status_bar(&self) -> Option<Box<dyn crate::window::StatusBar>> {
        Some(Box::new(self.bar.clone()))
    }
}
