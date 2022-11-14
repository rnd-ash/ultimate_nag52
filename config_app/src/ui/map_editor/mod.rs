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
        Layout, TextEdit, RichText, Response,
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
    ReadEEPROM = 0x08
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum MapViewType {
    EEPROM,
    Default,
    Modify,
}

#[derive(Debug, Clone)]
pub struct Map {
    meta: MapData,
    x_values: Vec<i16>,
    y_values: Vec<i16>,
    eeprom_key: String,
    /// EEPROM data
    data_eeprom: Vec<i16>,
    /// Map data in memory NOW
    data_memory: Vec<i16>,
    /// Program default map
    data_program: Vec<i16>,
    /// User editing map
    data_modify: Vec<i16>,
    showing_default: bool,
    ecu_ref: Arc<Mutex<Kwp2000DiagnosticServer>>,
    curr_edit_cell: Option<(usize, String, Response)>,
    view_type: MapViewType
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
        let mut eeprom : Vec<i16> = Vec::new();

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
        let (mut d_data, d_arr_size) = read_u16(ecu_response)?;
        if d_data.len() != d_arr_size as usize {
            return Err(DiagError::InvalidResponseLength);
        }
        for _ in 0..(d_arr_size / 2) {
            let (d, v) = read_i16(d_data)?;
            default.push(v);
            d_data = d;
        }
        
        let ecu_response = &s.send_byte_array_with_response(&[
            KWP2000Command::ReadDataByLocalIdentifier.into(),
            0x19,
            map_id,
            MapCmd::ReadEEPROM as u8,
            0x00,
            0x00,
        ])?[1..];
        drop(s); // Drop Mutex lock
        let (mut e_data, e_arr_size) = read_u16(ecu_response)?;
        if e_data.len() != e_arr_size as usize {
            return Err(DiagError::InvalidResponseLength);
        }
        for _ in 0..(e_arr_size / 2) {
            let (d, v) = read_i16(e_data)?;
            eeprom.push(v);
            e_data = d;
        }

        Ok(Self {
            x_values: x_elements,
            y_values: y_elements,
            eeprom_key: key,
            data_eeprom: eeprom,
            data_memory: current.clone(),
            data_program: default,
            data_modify: current,
            meta,
            showing_default: false,
            ecu_ref: server,
            curr_edit_cell: None,
            view_type: MapViewType::Modify

        })
    }

    fn data_to_byte_array(&self, data: &Vec<i16>) -> Vec<u8> {
        let mut ret = Vec::new();
        ret.extend_from_slice(&((data.len()*2) as u16).to_le_bytes());
        for point in data {
            ret.extend_from_slice(&point.to_le_bytes());
        }
        ret
    }

    pub fn write_to_ram(&self) -> DiagServerResult<()> {
        let mut payload: Vec<u8> = vec![
            KWP2000Command::WriteDataByLocalIdentifier.into(), 
            0x19, 
            self.meta.id,
            MapCmd::Write as u8,
        ];
        payload.extend_from_slice(&self.data_to_byte_array(&self.data_modify));
        self.ecu_ref.lock().unwrap().send_byte_array_with_response(&payload)?;
        Ok(())
    }

    pub fn save_to_eeprom(&self) -> DiagServerResult<()> {
        let payload: Vec<u8> = vec![
            KWP2000Command::WriteDataByLocalIdentifier.into(), 
            0x19, 
            self.meta.id,
            MapCmd::Burn as u8,
            0x00, 0x00
        ];
        self.ecu_ref.lock().unwrap().send_byte_array_with_response(&payload)?;
        Ok(())
    }

    pub fn undo_changes(&self) -> DiagServerResult<()> {
        let payload: Vec<u8> = vec![
            KWP2000Command::WriteDataByLocalIdentifier.into(), 
            0x19, 
            self.meta.id,
            MapCmd::Undo as u8,
            0x00, 0x00
        ];
        self.ecu_ref.lock().unwrap().send_byte_array_with_response(&payload)?;
        Ok(())
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

    fn gen_edit_table(&mut self, raw_ui: &mut egui::Ui) {
        let hash = match self.view_type {
            MapViewType::EEPROM => &self.data_eeprom,
            MapViewType::Default => &self.data_program,
            MapViewType::Modify => &self.data_modify,
        };
        let header_color = raw_ui.visuals().warn_fg_color;
        let cell_edit_color = raw_ui.visuals().error_fg_color;
        if !self.meta.x_desc.is_empty() {
            raw_ui.label(format!("X: {}", self.meta.x_desc));
        }
        if !self.meta.y_desc.is_empty() {
            raw_ui.label(format!("Y: {}", self.meta.y_desc));
        }
        if !self.meta.v_desc.is_empty() {
            raw_ui.label(format!("Values: {}", self.meta.v_desc));
        }
        let mut copy = self.clone();
        let resp = raw_ui.push_id(&hash, |ui| {
            let mut table_builder = egui_extras::TableBuilder::new(ui)
                .striped(true)
                .scroll(false)
                .cell_layout(Layout::left_to_right(egui::Align::Center).with_cross_align(egui::Align::Center))
                .column(Size::initial(60.0).at_least(60.0));
            for _ in 0..copy.x_values.len() {
                table_builder = table_builder.column(Size::initial(70.0).at_least(70.0));
            }
            table_builder.header(15.0, |mut header | {
                header.col(|_| {}); // Nothing in corner cell
                if copy.x_values.len() == 1 {
                    header.col(|_|{});
                } else {
                    for v in 0..copy.x_values.len() {
                        header.col(|u| {
                            u.label(RichText::new(format!("{}", copy.get_x_label(v))).color(header_color));
                        });
                    }
                }
            }).body(|body| {
                body.rows(15.0, copy.y_values.len(), |row_id, mut row| {
                    // Header column
                    row.col(|c| { c.label(RichText::new(format!("{}", copy.get_y_label(row_id))).color(header_color));});
                        
                    // Data columns
                    for x_pos in 0..copy.x_values.len() {
                        row.col(|cell| {
                            match self.view_type {
                                MapViewType::EEPROM => {
                                    cell.label(format!("{}", copy.data_eeprom[(row_id*copy.x_values.len())+x_pos]));
                                },
                                MapViewType::Default => {
                                    cell.label(format!("{}", copy.data_program[(row_id*copy.x_values.len())+x_pos]));
                                },
                                MapViewType::Modify => {
                                    let map_idx = (row_id*copy.x_values.len())+x_pos;
                                    let mut value = format!("{}", copy.data_modify[map_idx]);
                                    if let Some((curr_edit_idx, current_edit_txt, resp)) = &copy.curr_edit_cell {
                                        if *curr_edit_idx == map_idx {
                                            println!("Editing current cell {}", current_edit_txt);
                                            value = current_edit_txt.clone();
                                        }
                                    }
                                    let changed_value = value != format!("{}", copy.data_eeprom[map_idx]);
                                    let mut edit = TextEdit::singleline(&mut value);
                                    if changed_value {
                                        edit = edit.text_color(cell_edit_color);
                                    }
                                    let mut response = cell.add(edit);
                                    if changed_value {
                                        response = response.on_hover_text(format!("Current in EEPROM: {}", copy.data_eeprom[map_idx]));
                                    }
                                    if response.lost_focus() || cell.ctx().input().key_pressed(egui::Key::Enter) {
                                        println!("Cell ({},{}) lost focus, editing done", row_id, x_pos);
                                        if let Ok(new_v) = i16::from_str_radix(&value, 10) {
                                            copy.data_modify[map_idx] = new_v;
                                        }
                                        copy.curr_edit_cell = None;
                                    } else if response.gained_focus() || response.has_focus() {
                                        if let Some((curr_edit_idx, current_edit_txt, _resp)) = &copy.curr_edit_cell {
                                            if let Ok(new_v) = i16::from_str_radix(&current_edit_txt, 10) {
                                                copy.data_modify[*curr_edit_idx] = new_v;
                                            }
                                        }
                                        copy.curr_edit_cell = Some((map_idx, value, response));
                                    }
                                }
                            }
                        });
                    }
                })
            });
        });
        *self = copy;
    }

    fn generate_window_ui(&mut self, raw_ui: &mut egui::Ui) -> Option<PageAction> {
        raw_ui.label(format!("EEPROM key: {}", self.eeprom_key));
        raw_ui.label(format!(
            "Map has {} elements",
            self.x_values.len() * self.y_values.len()
        ));
        self.gen_edit_table(raw_ui);
        // Generate display chart
        if self.x_values.len() == 1 {
            // Bar chart
            let mut bars = Vec::new();
            for x in 0..self.y_values.len() {
                // Distinct points
                let value = match self.view_type {
                    MapViewType::Default => self.data_program[x],
                    MapViewType::EEPROM => self.data_eeprom[x],
                    MapViewType::Modify => self.data_modify[x]
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
                    let map_idx = (y_idx*self.x_values.len())+x_idx;
                    let data = match self.view_type {
                        MapViewType::Default => self.data_program[map_idx],
                        MapViewType::EEPROM => self.data_eeprom[map_idx],
                        MapViewType::Modify => self.data_modify[map_idx]
                    };
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
                .width(raw_ui.available_width())
                .show(raw_ui, |plot_ui| {
                    for l in lines {
                        plot_ui.line(l);
                    }
                });

        }
        raw_ui.label("View mode:");
        raw_ui.horizontal(|row| {
            row.selectable_value(&mut self.view_type, MapViewType::Modify, "User changes");
            row.selectable_value(&mut self.view_type, MapViewType::EEPROM, "EEPROM");
            row.selectable_value(&mut self.view_type, MapViewType::Default, "TCU default");
        });
        if self.data_modify != self.data_eeprom {
            if raw_ui.button("Undo user changes").clicked() {
                return match self.undo_changes() {
                    Ok(_) => {
                        self.data_modify = self.data_eeprom.clone();
                        Some(PageAction::SendNotification { text: format!("Map {} undo OK!", self.eeprom_key), kind: ToastKind::Success })
                    },
                    Err(e) => Some(PageAction::SendNotification { text: format!("Map {} undo failed! {}", self.eeprom_key, e), kind: ToastKind::Error })
                }
            }
            if raw_ui.button("Write changes (To RAM)").clicked() {
                return match self.write_to_ram() {
                    Ok(_) => {
                        self.data_memory = self.data_modify.clone();
                        Some(PageAction::SendNotification { text: format!("Map {} RAM write OK!", self.eeprom_key), kind: ToastKind::Success })
                    },
                    Err(e) => Some(PageAction::SendNotification { text: format!("Map {} RAM write failed! {}", self.eeprom_key, e), kind: ToastKind::Error })
                }
            }
        }
        if self.data_memory != self.data_eeprom {
            if raw_ui.button("Write changes (To EEPROM)").clicked() {
                return match self.save_to_eeprom() {
                    Ok(_) => {
                        if let Ok(new_data) = Self::new(self.meta.id, self.ecu_ref.clone(), self.meta) {
                            *self = new_data;
                        }
                        Some(PageAction::SendNotification { text: format!("Map {} EEPROM save OK!", self.eeprom_key), kind: ToastKind::Success })
                    },
                    Err(e) => Some(PageAction::SendNotification { text: format!("Map {} EEPROM save failed! {}", self.eeprom_key, e), kind: ToastKind::Error })
                }
            }
        }
        if self.data_modify != self.data_program {
            if raw_ui.button("Reset to flash defaults").clicked() {
                self.data_modify = self.data_program.clone();
            }
        }
        None
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
    v_desc: &'static str,
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
        v_desc: &'static str,
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
            v_desc,
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
        "Upshift RPM threshold",
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
        "Upshift RPM threshold",
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
        "Upshift RPM threshold",
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
        "Downshift RPM threshold",
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
        "Downshift RPM threshold",
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
        "Downshift RPM threshold",
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
        "Downshift RPM threshold",
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
        "Solenoid current (mA)",
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
        "Solenoid PWM duty (4096 = 100% on)",
        "/4096",
        None,
        None,
    ),
    MapData::new(
        0x0A,
        "Clutch filling time",
        "C",
        "",
        "ATF Temperature",
        "Clutch",
        "filling time in millseconds",
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
        "filling pressure in millibar",
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
        server.lock().unwrap().set_diagnostic_session_mode(SessionType::ExtendedDiagnostics);
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
        let mut action = None;
        for (key, map) in self.loaded_maps.iter_mut() {
            let mut open = true;
            egui::Window::new(map.meta.name)
                .auto_sized()
                .collapsible(true)
                .open(&mut open)
                .vscroll(false)
                .default_size(vec2(800.0, 400.0))
                .show(ui.ctx(), |window| {
                    action = map.generate_window_ui(window);
                });
            if !open {
                remove_list.push(key.clone())
            }
        }
        for key in remove_list {
            self.loaded_maps.remove(&key);
        }
        if let Some(act) = action {
            return act;
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
