//! Abstraction layer for ECU_Diagnostics crate
//! using the USB connection directly to the ESP32 chip
//! The USB endpoint ONLY supports sending fakes ISO-TP messages

use std::{
    io::{BufRead, BufReader, BufWriter, Write},
    sync::{
        atomic::{AtomicBool, Ordering},
        mpsc::{self},
        Arc,
    },
    time::{Instant, Duration}, panic::catch_unwind,
};
use std::fmt::Write as SWrite;
use ecu_diagnostics::{
    channel::{IsoTPChannel, PayloadChannel, ChannelError},
    hardware::{HardwareInfo, HardwareResult,  HardwareError},
};
use serial_rs::{SerialPort, PortInfo, SerialPortSettings, FlowControl};

#[derive(Debug, Clone, Copy)]
pub enum EspLogLevel {
    Debug,
    Info,
    Warn,
    Error,
    Unknown,
}

#[derive(Debug, Clone)]
pub struct EspLogMessage {
    pub lvl: EspLogLevel,
    pub timestamp: u128,
    pub tag: String,
    pub msg: String,
}

pub struct Nag52USB {
    port: Option<Box<dyn SerialPort>>,
    info: HardwareInfo,
    rx_diag: mpsc::Receiver<(u32, Vec<u8>)>,
    rx_log: Option<mpsc::Receiver<EspLogMessage>>,
    is_running: Arc<AtomicBool>,
    tx_id: u32,
    rx_id: u32,
    legacy_tx_mode: bool
}

unsafe impl Sync for Nag52USB {}
unsafe impl Send for Nag52USB {}

impl Nag52USB {
    pub fn new(path: &str, info: PortInfo, legacy_tx_mode: bool) -> HardwareResult<Self> {
        let mut port = serial_rs::new_from_path(path, Some(SerialPortSettings::default()
            .baud(921600)
            .read_timeout(Some(100))
            .write_timeout(Some(100))
            .set_flow_control(FlowControl::None)))
            .map_err(|e| HardwareError::APIError {
                code: 99,
                desc: e.to_string(),
            })?;

        let (read_tx_log, read_rx_log) = mpsc::channel::<EspLogMessage>();
        let (read_tx_diag, read_rx_diag) = mpsc::channel::<(u32, Vec<u8>)>();

        let is_running = Arc::new(AtomicBool::new(true));
        let is_running_r = is_running.clone();
        port.clear_input_buffer();
        port.clear_output_buffer();
        let mut port_clone = port.try_clone().unwrap();

        // Create 2 threads, one to read the port, one to write to it
        let reader_thread = std::thread::spawn(move || {
            println!("Serial reader start");
            while is_running_r.load(Ordering::Relaxed) {
                let mut reader = BufReader::new(&mut port_clone);
                let mut line = String::new();
                loop {
                    line.clear();
                    if reader.read_line(&mut line).is_ok() {      
                        line.pop();
                        if line.is_empty() {continue;}
                        //println!("LINE: {}", line);
                        if line.starts_with("#") || line.starts_with("07E9") {
                            // First char is #, diag message
                            // Diag message
                            if line.starts_with("#") {
                                line.remove(0);
                            }
                            if line.len() % 2 != 0 {
                                eprintln!("Discarding invalid diag msg '{}'", line);
                            } else {
                                let can_id = u32::from_str_radix(&line[0..4], 16).unwrap();
                                if let Ok(p) = catch_unwind(||{
                                    let payload: Vec<u8> = (4..line.len())
                                        .step_by(2)
                                        .map(|i| u8::from_str_radix(&line[i..i + 2], 16).unwrap())
                                        .collect();
                                    payload
                                }) {
                                    read_tx_diag.send((can_id, p));
                                }
                            }
                        } else {
                            println!("{}", line );
                            //read_tx_log.send(msg);
                        }
                        line.clear();
                    }
                }
            }
            println!("Serial reader stop");
        });

        Ok(Self {
            port: Some(port),
            is_running,
            info: HardwareInfo {
                name: "Ultimate-Nag52 USB interface".to_string(),
                vendor: Some("rnd-ash@github.com".to_string()),
                device_fw_version: None,
                api_version: None,
                library_version: None,
                library_location: None,
                capabilities: ecu_diagnostics::hardware::HardwareCapabilities {
                    iso_tp: true,
                    can: false,
                    kline: false,
                    kline_kwp: false,
                    sae_j1850: false,
                    sci: false,
                    ip: false,
                },
            },
            rx_diag: read_rx_diag,
            rx_log: Some(read_rx_log),
            tx_id: 0,
            rx_id: 0,
            legacy_tx_mode
        })
    }

    pub fn is_connected(&self) -> bool {
        self.is_running.load(Ordering::Relaxed)
    }

    pub fn get_logger_receiver(&mut self) -> Option<mpsc::Receiver<EspLogMessage>> {
        self.rx_log.take()
    }
}

impl Drop for Nag52USB {
    fn drop(&mut self) {
        self.is_running.store(false, Ordering::Relaxed);
    }
}

impl ecu_diagnostics::hardware::Hardware for Nag52USB {
    fn create_iso_tp_channel(
        this: std::sync::Arc<std::sync::Mutex<Self>>,
    ) -> ecu_diagnostics::hardware::HardwareResult<Box<dyn ecu_diagnostics::channel::IsoTPChannel>>
    {
        Ok(Box::new(this.clone()))
    }

    fn create_can_channel(
        _this: std::sync::Arc<std::sync::Mutex<Self>>,
    ) -> ecu_diagnostics::hardware::HardwareResult<Box<dyn ecu_diagnostics::channel::CanChannel>>
    {
        Err(HardwareError::ChannelNotSupported)
    }

    fn is_iso_tp_channel_open(&self) -> bool {
        true
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
        match self.port {
            Some(_) => Ok(()),
            None => Err(ChannelError::InterfaceNotOpen),
        }
    }

    fn close(&mut self) -> ecu_diagnostics::channel::ChannelResult<()> {
        match self.port {
            Some(_) => Ok(()),
            None => Err(ChannelError::InterfaceNotOpen),
        }
    }

    fn set_ids(&mut self, send: u32, recv: u32) -> ecu_diagnostics::channel::ChannelResult<()> {
        self.tx_id = send;
        self.rx_id = recv;
        Ok(())
    }

    fn read_bytes(&mut self, timeout_ms: u32) -> ecu_diagnostics::channel::ChannelResult<Vec<u8>> {
        let now = Instant::now();
        while now.elapsed().as_millis() < timeout_ms as u128 {
            if let Ok((id, data)) = self.rx_diag.try_recv() {
                if id == self.rx_id {
                    return Ok(data);
                }
            }
        }
        return Err(ChannelError::BufferEmpty);
    }

    fn write_bytes(
        &mut self,
        addr: u32,
        buffer: &[u8],
        _timeout_ms: u32,
    ) -> ecu_diagnostics::channel::ChannelResult<()> {
        // Just write buffer
        match self.port.as_mut() {
            Some(p) => {
                if self.legacy_tx_mode {
                let mut buf = String::with_capacity(buffer.len()*2 + 6);
                write!(buf, "#{:04X}", addr);
                for x in buffer {
                    write!(buf, "{:02X}", x);
                }
                write!(buf, "\n");
                p.write_all(buf.as_bytes()).map_err(|e| ChannelError::IOError(e))?;
            } else {
                let mut to_write = Vec::with_capacity(buffer.len()+4);
                let size: u16 = (buffer.len()+2) as u16;
                to_write.push((size >> 8) as u8);
                to_write.push((size & 0xFF) as u8);
                to_write.push((addr >> 8) as u8);
                to_write.push((addr & 0xFF) as u8);
                to_write.extend_from_slice(&buffer);
                p.write_all(&to_write)?;
            }
                Ok(())
            }
            None => Err(ChannelError::InterfaceNotOpen),
        }
    }

    fn clear_rx_buffer(&mut self) -> ecu_diagnostics::channel::ChannelResult<()> {
        match self.port.as_mut() {
            Some(p) => {
                while self.rx_diag.try_recv().is_ok() {} // Clear rx_diag too!
                Ok(())
            }
            None => Err(ChannelError::InterfaceNotOpen),
        }
    }

    fn clear_tx_buffer(&mut self) -> ecu_diagnostics::channel::ChannelResult<()> {
        Ok(())
    }
}

impl IsoTPChannel for Nag52USB {
    fn set_iso_tp_cfg(
        &mut self,
        _cfg: ecu_diagnostics::channel::IsoTPSettings,
    ) -> ecu_diagnostics::channel::ChannelResult<()> {
        Ok(()) // Don't care
    }
}
