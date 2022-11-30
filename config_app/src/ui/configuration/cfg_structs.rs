use std::fmt::Display;

use modular_bitfield::{bitfield, BitfieldSpecifier};

#[bitfield]
#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub struct TcmCoreConfig {
    pub is_large_nag: u8,
    pub diff_ratio: u16,
    pub wheel_circumference: u16,
    pub is_four_matic: u8,
    pub transfer_case_high_ratio: u16,
    pub transfer_case_low_ratio: u16,
    pub default_profile: DefaultProfile,
    pub red_line_dieselrpm: u16,
    pub red_line_petrolrpm: u16,
    pub engine_type: EngineType,
    pub egs_can_type: EgsCanType,
    // Only for V1,2 and newer PCBs
    pub shifter_style: ShifterStyle,
    // Only for V1.3 and newer PCBs
    pub io_0_usage: IOPinConfig,
    pub input_sensor_pulses_per_rev: u8,
    pub output_pulse_width_per_kmh: u8,
    pub mosfet_purpose: MosfetPurpose
}

#[derive(BitfieldSpecifier)]
#[bits = 8]
#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub enum EgsCanType {
    UNKNOWN = 0,
    EGS51 = 1,
    EGS52 = 2,
    EGS53 = 3,
}

#[derive(BitfieldSpecifier)]
#[bits = 8]
#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub enum ShifterStyle {
    EWM_CAN = 0,
    TRRS = 1,
    SLR_MCLAREN = 2, // NEEDS TESTING (Need to work out how this works)
}

#[derive(BitfieldSpecifier)]
#[bits = 8]
#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub enum IOPinConfig {
    NotConnected = 0,
    Input = 1,
    Output = 2
}

#[derive(BitfieldSpecifier)]
#[bits = 8]
#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub enum MosfetPurpose {
    NotConnected = 0,
    TorqueCutTrigger = 1,
    B3BrakeSolenoid = 2
}

#[bitfield]
#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub struct TcmEfuseConfig {
    pub board_ver: BoardType,
    pub manf_day: u8,
    pub manf_week: u8,
    pub manf_month: u8,
    pub manf_year: u8
}

#[derive(BitfieldSpecifier)]
#[bits = 8]
#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub enum DefaultProfile {
    Standard = 0,
    Comfort = 1,
    Winter = 2,
    Agility = 3,
    Manual = 4,
}

#[derive(BitfieldSpecifier)]
#[bits = 8]
#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub enum EngineType {
    Diesel,
    Petrol,
}

#[derive(BitfieldSpecifier)]
#[bits = 8]
#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub enum BoardType {
    Unknown = 0,
    V11 = 1,
    V12 = 2,
    V13 = 3
}

impl Display for BoardType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            BoardType::Unknown => write!(f, "Unknown"),
            BoardType::V11 => write!(f, "V1.1 (12/12/21)"),
            BoardType::V12 => write!(f, "V1.2 (07/07/22)"),
            BoardType::V13 => write!(f, "V1.3 (UNDER DEVELOPMENT DO NOT SELECT)"),
        }
    }
}

impl Into<String> for BoardType {
    fn into(self) -> String {
        format!("{}", self)
    }
}
