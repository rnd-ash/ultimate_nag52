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
