//! Abstraction layer for ECU_Diagnostics crate
//! using the USB connection directly to the ESP32 chip
//! The USB endpoint ONLY supports sending fakes ISO-TP messages

use std::{
    io::{BufRead, BufReader, BufWriter},
    sync::{
        atomic::{AtomicBool, Ordering},
        mpsc::{self, Receiver},
        Arc,
    },
    time::Instant,
};

use ecu_diagnostics::{
    channel::{IsoTPChannel, PayloadChannel},
    hardware::{HardwareInfo, HardwareResult},
    ChannelError, HardwareError,
};
use serialport::{ClearBuffer, DataBits, FlowControl, Parity, SerialPortBuilder, StopBits};

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
    port: Option<Box<dyn serialport::SerialPort>>,
    port_name: String,
    info: HardwareInfo,
    rx_log: mpsc::Receiver<EspLogMessage>,
    rx_diag: mpsc::Receiver<(u32, Vec<u8>)>,
    tx: mpsc::Sender<Vec<u8>>,
    is_running: Arc<AtomicBool>,
    is_running_reader: Arc<AtomicBool>,
    is_running_writer: Arc<AtomicBool>,
    tx_id: u32,
    rx_id: u32,
}

unsafe impl Sync for Nag52USB {}
unsafe impl Send for Nag52USB {}

impl Nag52USB {
    pub fn new(path: &str) -> HardwareResult<Self> {
        let port = serialport::new(path, 256000)
            .flow_control(FlowControl::None)
            .data_bits(DataBits::Eight)
            .timeout(std::time::Duration::from_millis(200))
            .stop_bits(StopBits::One)
            .parity(Parity::None)
            .open()
            .map_err(|e| HardwareError::APIError {
                code: 99,
                desc: e.description,
            })?;
        port.clear(ClearBuffer::All)
            .map_err(|e| HardwareError::APIError {
                code: 99,
                desc: format!("Clearbuffer error {}", e.description),
            })?;

        let (read_tx_log, read_rx_log) = mpsc::channel::<EspLogMessage>();
        let (read_tx_diag, read_rx_diag) = mpsc::channel::<(u32, Vec<u8>)>();
        let (send_tx, send_rx) = mpsc::channel::<Vec<u8>>();

        let mut reader = BufReader::new(port.try_clone().unwrap());
        let mut writer = BufWriter::new(port.try_clone().unwrap());

        let is_running = Arc::new(AtomicBool::new(true));
        let is_running_r = is_running.clone();
        let is_running_w = is_running.clone();

        let is_running_reader = Arc::new(AtomicBool::new(true));
        let is_running_reader_t = is_running.clone();

        let is_running_writer = Arc::new(AtomicBool::new(true));
        let is_running_writer_t = is_running.clone();

        // Create 2 threads, one to read the port, one to write to it
        let reader_thread = std::thread::spawn(move || {
            println!("Serial reader start");
            is_running_reader_t.store(true, Ordering::Relaxed);
            let mut buffer = String::new();
            while is_running_r.load(Ordering::Relaxed) {
                if let Ok(n) = reader.read_line(&mut buffer) {
                    if n != 0 {
                        if buffer.chars().next().unwrap() == '#' {
                            // First char is #, diag message
                            // Diag message
                            if buffer.len() % 2 != 0 {
                                eprintln!("Discarding invalid diag msg {}", buffer);
                            } else {
                                println!("Read diag payload {:?} from Nag52 USB", buffer);
                                let contents: &str = &buffer[1..buffer.len() - 1];
                                let can_id = u32::from_str_radix(&contents[0..4], 16).unwrap();
                                let payload: Vec<u8> = (4..contents.len())
                                    .step_by(2)
                                    .map(|i| u8::from_str_radix(&contents[i..i + 2], 16).unwrap())
                                    .collect();
                                read_tx_diag.send((can_id, payload));
                            }
                        } else {
                            // This is a log message
                            if buffer.len() < 5 {
                                break;
                            }
                            buffer.truncate(buffer.len() - 2); // Remove \r\n
                            let lvl = match buffer.chars().next().unwrap() {
                                'I' => EspLogLevel::Info,
                                'W' => EspLogLevel::Warn,
                                'E' => EspLogLevel::Error,
                                'D' => EspLogLevel::Debug,
                                _ => break,
                            };
                            let timestamp = u128::from_str_radix(
                                buffer.split_once(")").unwrap().0.split_once("(").unwrap().1,
                                10,
                            )
                            .unwrap();
                            let split = buffer.split_once(": ").unwrap();
                            let msg = EspLogMessage {
                                lvl,
                                timestamp,
                                tag: split.0.split_once(")").unwrap().1.to_string(),
                                msg: split.1.to_string(),
                            };
                            read_tx_log.send(msg);
                        }
                        buffer.clear();
                    }
                }
                std::thread::sleep(std::time::Duration::from_millis(10));
            }
            println!("Serial reader stop");
            is_running_reader_t.store(false, Ordering::Relaxed);
        });
        Ok(Self {
            port: Some(port),
            port_name: path.into(),
            is_running,
            is_running_reader,
            is_running_writer,
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
            rx_log: read_rx_log,
            rx_diag: read_rx_diag,
            tx: send_tx,
            tx_id: 0,
            rx_id: 0,
        })
    }

    pub fn read_log_msg(&self) -> Option<EspLogMessage> {
        self.rx_log.try_recv().ok()
    }

    pub fn is_connected(&self) -> bool {
        self.is_running_reader.load(Ordering::Relaxed)
            || self.is_running_writer.load(Ordering::Relaxed)
    }

    pub fn get_usb_path(&self) -> &str {
        &self.port_name
    }

    pub fn disconnect(&mut self) {
        self.is_running.store(false, Ordering::Relaxed);
        std::mem::drop(self.port.as_ref());
    }

    pub fn reconnect(&mut self) -> HardwareResult<()> {
        self.disconnect();
        let tmp = Self::new(&self.port_name)?;
        let _ = std::mem::replace(self, tmp);
        Ok(())
    }
}

impl Drop for Nag52USB {
    fn drop(&mut self) {
        self.disconnect();
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
            if let Ok((id, data)) = self.rx_diag.recv() {
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
                let mut buf = format!("#{:04X}", addr);
                for x in buffer {
                    buf.push_str(&format!("{:02X}", x));
                }
                buf.push('\n');
                println!("Sending {:?} to Nag52 USB", buf);
                p.write_all(&buf.as_bytes())
                    .map_err(|e| ChannelError::IOError(e));
                Ok(())
            }
            None => Err(ChannelError::InterfaceNotOpen),
        }
    }

    fn clear_rx_buffer(&mut self) -> ecu_diagnostics::channel::ChannelResult<()> {
        match self.port.as_mut() {
            Some(p) => {
                println!("Clearing Rx buffer");
                while self.rx_diag.try_recv().is_ok() {} // Clear rx_diag too!
                println!("Clearing Rx buffer done");
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
