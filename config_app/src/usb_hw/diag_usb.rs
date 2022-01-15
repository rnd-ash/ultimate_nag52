//! Abstraction layer for ECU_Diagnostics crate
//! using the USB connection directly to the ESP32 chip
//! The USB endpoint ONLY supports sending fakes ISO-TP messages

use ecu_diagnostics::{HardwareError, hardware::{HardwareInfo, HardwareResult}, channel::{IsoTPChannel, PayloadChannel}};
use serialport::{FlowControl, ClearBuffer, SerialPortBuilder, DataBits, Parity, StopBits};



pub struct Nag52USB {
    port: Box<dyn serialport::SerialPort>,
    info: HardwareInfo,
}

unsafe impl Sync for Nag52USB{}
unsafe impl Send for Nag52USB{}


impl Nag52USB {
    pub fn new(path: String, baud_rate: u32) -> HardwareResult<Self> {
        let mut port = match serialport::new(path, baud_rate)
            .flow_control(FlowControl::None)
            .baud_rate(256000)
            .data_bits(DataBits::Eight)
            .timeout(std::time::Duration::from_millis(200))
            .stop_bits(StopBits::One)
            .parity(Parity::None)
            .open() {
            Ok(mut p) => {
                p.clear(ClearBuffer::All).map_err(|e| HardwareError::APIError { code: 99, desc: format!("Clearbuffer error {}", e.description) })?;
            },
            Err(e) => return Err(HardwareError::APIError { code: 99, desc: e.description })
        };
        // Create 2 threads
    }
}


impl ecu_diagnostics::hardware::Hardware for Nag52USB {
    fn create_iso_tp_channel(this: std::sync::Arc<std::sync::Mutex<Self>>) -> ecu_diagnostics::hardware::HardwareResult<Box<dyn ecu_diagnostics::channel::IsoTPChannel>> {
        todo!()
    }

    fn create_can_channel(_this: std::sync::Arc<std::sync::Mutex<Self>>) -> ecu_diagnostics::hardware::HardwareResult<Box<dyn ecu_diagnostics::channel::CanChannel>> {
        Err(HardwareError::ChannelNotSupported)
    }

    fn is_iso_tp_channel_open(&self) -> bool {
        todo!()
    }

    fn is_can_channel_open(&self) -> bool {
        false
    }

    fn read_battery_voltage(&mut self) -> Option<f32> {
        None
    }

    fn read_ignition_voltage(&mut self) -> Option<f32> {
        None
    }

    fn get_info(&self) -> &ecu_diagnostics::hardware::HardwareInfo {
        &self.info
    }
}

impl PayloadChannel for Nag52USB {
    fn open(&mut self) -> ecu_diagnostics::channel::ChannelResult<()> {
        todo!()
    }

    fn close(&mut self) -> ecu_diagnostics::channel::ChannelResult<()> {
        todo!()
    }

    fn set_ids(&mut self, send: u32, recv: u32) -> ecu_diagnostics::channel::ChannelResult<()> {
        todo!()
    }

    fn read_bytes(&mut self, timeout_ms: u32) -> ecu_diagnostics::channel::ChannelResult<Vec<u8>> {
        todo!()
    }

    fn write_bytes(&mut self, addr: u32, buffer: &[u8], timeout_ms: u32) -> ecu_diagnostics::channel::ChannelResult<()> {
        todo!()
    }

    fn clear_rx_buffer(&mut self) -> ecu_diagnostics::channel::ChannelResult<()> {
        todo!()
    }

    fn clear_tx_buffer(&mut self) -> ecu_diagnostics::channel::ChannelResult<()> {
        todo!()
    }
}

impl IsoTPChannel for Nag52USB {
    fn set_iso_tp_cfg(&mut self, cfg: ecu_diagnostics::channel::IsoTPSettings) -> ecu_diagnostics::channel::ChannelResult<()> {
        todo!()
    }
}