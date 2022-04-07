//! Read data by local identifier data structures
//! Based on diag_data.h in TCM source code

use std::mem::size_of;
use ecu_diagnostics::{DiagError, DiagServerResult};
use ecu_diagnostics::kwp2000::{Kwp2000DiagnosticServer};
use egui::{Color32, InnerResponse, RichText, Ui};

#[repr(u8)]
#[derive(Copy, Clone, Ord, PartialOrd, Eq, PartialEq, Debug)]
pub enum RecordIdents {
    GearboxSensors = 0x20,
    SolenoidStatus = 0x21,
    CanDataDump = 0x22,
}

impl RecordIdents {
    pub fn query_ecu(&self, server: &mut Kwp2000DiagnosticServer) -> DiagServerResult<LocalRecordData> {
        let resp = server.read_custom_local_identifier(*self as u8)?;
        let expected_size = match &self {
            Self::GearboxSensors => size_of::<DataGearboxSensors>(),
            Self::SolenoidStatus => size_of::<DataSolenoids>(),
            Self::CanDataDump => size_of::<DataCanDump>(),
        };
        if resp.len() != expected_size {
            return Err(DiagError::InvalidResponseLength)
        }
        match self {
            Self::GearboxSensors => Ok(LocalRecordData::Sensors(unsafe { std::ptr::read(resp.as_ptr() as *const _) })),
            Self::SolenoidStatus => Ok(LocalRecordData::Solenoids(unsafe { std::ptr::read(resp.as_ptr() as *const _) })),
            Self::CanDataDump => Ok(LocalRecordData::Canbus(unsafe { std::ptr::read(resp.as_ptr() as *const _) })),
        }
    }
}

#[derive(Debug, Copy, Clone, Ord, PartialOrd, Eq, PartialEq)]
pub enum LocalRecordData {
    Sensors(DataGearboxSensors),
    Solenoids(DataSolenoids),
    Canbus(DataCanDump)
}

impl LocalRecordData {
    pub fn to_table(&self, ui: &mut Ui) -> InnerResponse<()> {
        match &self {
            LocalRecordData::Sensors(s) => s.to_table(ui),
            LocalRecordData::Solenoids(s) => s.to_table(ui),
            LocalRecordData::Canbus(s) => s.to_table(ui)
        }
    }

    pub fn get_chart_data(&self) -> Vec<ChartData> {
        match &self {
            LocalRecordData::Sensors(s) => s.to_chart_data(),
            LocalRecordData::Solenoids(s) => s.to_chart_data(),
            LocalRecordData::Canbus(s) => s.to_chart_data(),
            _ => vec![]
        }
    }
}

#[repr(C, packed)]
#[derive(Debug, Copy, Clone, Ord, PartialOrd, Eq, PartialEq)]
pub struct DataGearboxSensors {
    pub n2_rpm: u16,
    pub n3_rpm: u16,
    pub calculated_rpm: u16,
    pub calc_ratio: u16,
    pub v_batt: u16,
    pub atf_temp_c: i32,
    pub parking_lock: u8
}

fn make_text<T: Into<String>>(t: T, e: bool) -> egui::RichText {
    let mut s = RichText::new(t);
    if e {
        s = s.color(Color32::from_rgb(255,0,0))
    }
    s
}

impl DataGearboxSensors {
    pub fn to_table(&self, ui: &mut Ui) -> InnerResponse<()> {
        egui::Grid::new("DGS").striped(true).show(ui, |ui| {
            ui.label("N2 Pulse counter").on_hover_text("Raw counter value for PCNT for N2 hall effect RPM sensor");
            ui.label(if self.n2_rpm == u16::MAX { make_text("ERROR", true) } else { make_text(format!("{} pulses/min", self.n2_rpm), false) });
            ui.end_row();

            ui.label("N3 Pulse counter").on_hover_text("Raw counter value for PCNT for N3 hall effect RPM sensor");
            ui.label(if self.n3_rpm == u16::MAX { make_text("ERROR", true) } else { make_text(format!("{} pulses/min", self.n3_rpm), false) });
            ui.end_row();

            ui.label("Calculated input RPM").on_hover_text("Calculated input shaft RPM based on N2 and N3 raw values");
            ui.label(if self.calculated_rpm == u16::MAX { make_text("ERROR", true) } else { make_text(format!("{} RPM", self.calculated_rpm), false) });
            ui.end_row();

            ui.label("Calculated ratio").on_hover_text("Calculated gear ratio");
            ui.label(if self.calculated_rpm == u16::MAX { make_text("ERROR", true) } else { make_text(format!("{:.2}", self.calc_ratio as f32 / 100.0), false) });
            ui.end_row();

            ui.label("Battery voltage");
            ui.label(if self.v_batt == u16::MAX { make_text("ERROR", true) } else { make_text(format!("{:.1} V", self.v_batt as f32/1000.0), false) });
            ui.end_row();

            ui.label("ATF Oil temperature\n(Only when parking lock off)");
            ui.label(if self.atf_temp_c == u16::MAX as i32 { make_text("Cannot read\nParking lock engaged", true) } else { make_text(format!("{} *C", self.atf_temp_c), false) });
            ui.end_row();

            ui.label("Parking lock");
            ui.label(if self.parking_lock == 0x00 { make_text("No", false) } else { make_text("Yes", false) });
            ui.end_row();
        })
    }

    pub fn to_chart_data(&self) -> Vec<ChartData> {
        vec![
            ChartData::new(
                "RPM sensors".into(),
                vec![
                    ("N2 raw", self.n2_rpm as f32, None),
                    ("N3 raw", self.n3_rpm as f32, None),
                    ("Calculated RPM", self.calculated_rpm as f32, None),
                ],
                Some((0.0, 0.0))
            ),
        ]
    }
}

#[derive(Debug, Clone, PartialEq, PartialOrd)]
pub struct ChartData {
    /// Min, Max
    pub bounds: Option<(f32, f32)>,
    pub group_name: String,
    pub data: Vec<(String, f32, Option<String>)> // Data field name, data field value, data field unit
}

impl ChartData {
    pub fn new<T: Into<String>>(group_name: String, data: Vec<(T, f32, Option<T>)>, bounds: Option<(f32, f32)>) -> Self {
        Self { bounds, group_name, data: data.into_iter().map(|(n, v, u)| (n.into(), v, u.map(|x| x.into()))).collect() }
    }
}

#[repr(C, packed)]
#[derive(Debug, Copy, Clone, Ord, PartialOrd, Eq, PartialEq)]
pub struct DataSolenoids {
    pub spc_pwm: u16,
    pub mpc_pwm: u16,
    pub tcc_pwm: u16,
    pub y3_pwm: u16,
    pub y4_pwm: u16,
    pub y5_pwm: u16,
    pub spc_current: u16,
    pub mpc_current: u16,
    pub tcc_current: u16,
    pub y3_current: u16,
    pub y4_current: u16,
    pub y5_current: u16
}

impl DataSolenoids {
    pub fn to_table(&self, ui: &mut Ui) -> InnerResponse<()> {
        egui::Grid::new("DGS").striped(true).show(ui, |ui| {
            ui.label("MPC Solenoid");
            ui.label(format!("PWM {:>4}/4096, Est current {} mA", self.mpc_pwm, self.mpc_current));
            ui.end_row();

            ui.label("SPC Solenoid");
            ui.label(format!("PWM {:>4}/4096, Est current {} mA", self.spc_pwm, self.spc_current));
            ui.end_row();

            ui.label("TCC Solenoid");
            ui.label(format!("PWM {:>4}/4096, Est current {} mA", self.tcc_pwm, self.tcc_current));
            ui.end_row();

            ui.label("Y3 shift Solenoid");
            ui.label(format!("PWM {:>4}/4096, Est current {} mA", self.y3_pwm, self.y3_current));
            ui.end_row();

            ui.label("Y4 shift Solenoid");
            ui.label(format!("PWM {:>4}/4096, Est current {} mA", self.y4_pwm, self.y4_current));
            ui.end_row();

            ui.label("Y5 shift Solenoid");
            ui.label(format!("PWM {:>4}/4096, Est current {} mA", self.y5_pwm, self.y5_current));
            ui.end_row();

            ui.label("Total current consumption");
            ui.label(format!("{} mA",
                             self.y5_current +
                             self.y4_current +
                             self.y3_current +
                             self.mpc_current +
                             self.spc_current +
                             self.tcc_current
            ));
            ui.end_row();
        })
    }

    pub fn to_chart_data(&self) -> Vec<ChartData> {
        vec![
            ChartData::new(
                "Solenoid PWM".into(),
                vec![
                    ("MPC Solenoid", self.mpc_pwm as f32, None),
                    ("SPC Solenoid", self.spc_pwm as f32, None),
                    ("TCC Solenoid", self.tcc_pwm as f32, None),
                    ("Y3 Solenoid", self.y3_pwm as f32, None),
                    ("Y4 Solenoid", self.y4_pwm as f32, None),
                    ("Y5 Solenoid", self.y5_pwm as f32, None),
                ],
                Some((0.0, 4096.0))
            ),
            ChartData::new(
                "Solenoid Current".into(),
                vec![
                    ("MPC Solenoid", self.mpc_current as f32, Some("mA")),
                    ("SPC Solenoid", self.spc_current as f32, Some("mA")),
                    ("TCC Solenoid", self.tcc_current as f32, Some("mA")),
                    ("Y3 Solenoid", self.y3_current as f32, Some("mA")),
                    ("Y4 Solenoid", self.y4_current as f32, Some("mA")),
                    ("Y5 Solenoid", self.y5_current as f32, Some("mA")),
                ],
                Some((0.0, 6600.0))
            ),
        ]
    }
}

#[repr(u8)]
#[derive(Debug, Copy, Clone, Ord, PartialOrd, Eq, PartialEq)]
pub enum PaddlePosition {
    None,
    Plus,
    Minus,
    PlusAndMinus,
    SNV = 0xFF
}

#[repr(u8)]
#[derive(Debug, Copy, Clone, Ord, PartialOrd, Eq, PartialEq)]
pub enum ShifterPosition {
    Park,
    ParkReverse,
    Reverse,
    ReverseNeutral,
    Neutral,
    NeutralDrive,
    Drive,
    Plus,
    Minus,
    SNV = 0xFF
}

#[repr(C, packed)]
#[derive(Debug, Copy, Clone, Ord, PartialOrd, Eq, PartialEq)]
pub struct DataCanDump {
    pub pedal_position: u8,
    pub min_torque_ms: u16,
    pub max_torque_ms: u16,
    pub static_torque: u16,
    pub left_rear_rpm: u16,
    pub right_rear_rpm: u16,
    pub shift_profile_pressed: u8,
    pub selector_position: ShifterPosition,
    pub paddle_position: PaddlePosition
}

impl DataCanDump {
    pub fn to_table(&self, ui: &mut Ui) -> InnerResponse<()> {
        egui::Grid::new("DGS").striped(true).show(ui, |ui| {
            ui.label("Accelerator pedal position");
            ui.label(if self.pedal_position == u8::MAX { make_text("Signal not available", true) } else { make_text(format!("{:.1} %", self.pedal_position as f32 / 250.0 * 100.0), false) });
            ui.end_row();

            ui.label("Engine minimum torque");
            ui.label(if self.min_torque_ms == u16::MAX { make_text("Signal not available", true) } else { make_text(format!("{:.1} Nm", self.min_torque_ms as f32 / 4.0 - 500.0), false) });
            ui.end_row();

            ui.label("Engine maximum torque");
            ui.label(if self.max_torque_ms == u16::MAX { make_text("Signal not available", true) } else { make_text(format!("{:.1} Nm", self.max_torque_ms as f32 / 4.0 - 500.0), false) });
            ui.end_row();

            ui.label("Engine static torque");
            ui.label(if self.static_torque == u16::MAX { make_text("Signal not available", true) } else { make_text(format!("{:.1} Nm", self.static_torque as f32 / 4.0 - 500.0), false) });
            ui.end_row();

            ui.label("Rear left wheel speed");
            ui.label(if self.right_rear_rpm == u16::MAX { make_text("Signal not available", true) } else { make_text(format!("{:.1} RPM", self.right_rear_rpm as f32 / 2.0), false) });
            ui.end_row();

            ui.label("Left left wheel speed");
            ui.label(if self.left_rear_rpm == u16::MAX { make_text("Signal not available", true) } else { make_text(format!("{:.1} RPM", self.left_rear_rpm as f32 / 2.0), false) });
            ui.end_row();

            ui.label("Gear selector position");
            ui.label(if self.selector_position == ShifterPosition::SNV { make_text("Signal not available", true) } else { make_text(format!("{:?}", self.selector_position), false) });
            ui.end_row();

            ui.label("Shift paddle position");
            ui.label(if self.paddle_position == PaddlePosition::SNV { make_text("Signal not available", true) } else { make_text(format!("{:?}", self.paddle_position), false) });
            ui.end_row();
        })
    }

    pub fn to_chart_data(&self) -> Vec<ChartData> {
        let max = if self.max_torque_ms == u16::MAX {
            0.0
        } else {
            self.max_torque_ms as f32 / 4.0 - 500.0
        };
        let min = if self.min_torque_ms == u16::MAX {
            0.0
        } else {
            self.min_torque_ms as f32 / 4.0 - 500.0
        };
        let sta = if self.static_torque == u16::MAX {
            0.0
        } else {
            self.static_torque as f32 / 4.0 - 500.0
        };
        vec![
            ChartData::new(
                "Engine torque".into(),
                vec![
                    ("Max", max, None),
                    ("Min", min, None),
                    ("Static", sta, None),
                ],
                None
            ),
        ]
    }
}