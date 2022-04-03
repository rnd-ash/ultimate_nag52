use serial_rs::{SerialPort, FlowControl};

use super::slip_reader::SlipReader;



pub const CHIP_NAME: &str = "Espressif device";
pub const IS_STUB: bool = false;

// Commands
pub const ESP_SPI_SET_PARAMS: u8 = 0x0B;
pub const ESP_SPI_ATTACH: u8 = 0x0D;
pub const ESP_SYNC: u8 = 0x08;
pub const ROM_INVALID_RECV_MSG: u8 = 0x05;

pub const DEFAULT_TIMEOUT: u64 = 3000;

pub struct EspLoader {
    port: Box<dyn SerialPort>,
    slip_reader: Option<SlipReader>,
    sync_stub_detected: bool,
    dtr: bool,
}

impl EspLoader {
    pub fn new(mut port: Box<dyn SerialPort>) -> Self {
        // Before we init Slip reader, set baud
        let reader = SlipReader::new(port.try_clone().unwrap());
        Self {
            port,
            slip_reader: Some(reader),
            sync_stub_detected: false,
            dtr: false
        }
    }

    fn flush_input(&mut self) {
        self.port.clear_input_buffer();
        let _ = self.slip_reader.replace(SlipReader::new(self.port.try_clone().unwrap()));
    }

    fn reset_to_bootloader(&mut self, extra_delay: bool) {
        println!("Going to bootloader {}", extra_delay);
        self.port.set_data_terminal_ready(false);
        self.port.set_request_to_send(true);
        std::thread::sleep(std::time::Duration::from_millis(100));
        self.port.set_data_terminal_ready(true);
        self.port.set_request_to_send(false);
        std::thread::sleep(std::time::Duration::from_millis(if extra_delay { 500 } else {50}));
        self.port.set_data_terminal_ready(false);
    }

    pub fn connect(&mut self, attempts: u32) -> bool {
        let mut use_extra_delay = false;
        for attempt_idx in 0..attempts {
            if self.connect_attempt(use_extra_delay) {
                return true;
            }
            use_extra_delay = !use_extra_delay;
        }
        return false;
    }

    fn connect_attempt(&mut self, extra_delay: bool) -> bool {
        std::thread::sleep(std::time::Duration::from_millis(50));
        self.port.clear_input_buffer();
        self.port.clear_output_buffer();
        let _ = self.slip_reader.take(); // Don't need SLIP reader for this!
        self.reset_to_bootloader(extra_delay);
        let waiting = self.port.bytes_to_read().unwrap() as usize;
        let mut buffer: Vec<u8> = vec![0; waiting];
        self.port.read_exact(&mut buffer);
        let buffer_as_string = String::from_utf8_lossy(&buffer);
        println!("Buffer {}", buffer_as_string);
        if !buffer_as_string.to_string().contains("waiting for download") {
            println!("ESP not in download mode!");
            std::thread::sleep(std::time::Duration::from_millis(1000));
            return false;
        }
        for x in 0..5 {
            self.flush_input();
            self.port.clear_input_buffer();
            self.port.clear_output_buffer();
            if self.sync() {
                return true;
            } else {
                std::thread::sleep(std::time::Duration::from_millis(100));
            }
        }
        return false;
    }

    fn sync(&mut self) -> bool {
        println!("Trying sync");
        let mut buffer: Vec<u8> = vec![0x55; 32+4];
        buffer[0..4].copy_from_slice(&[0x07, 0x07, 0x12, 0x20]);
        if let Some((val, data)) = self.command(
            Some(ESP_SYNC),
            &buffer,
            0,
            true,
            100
        ).unwrap_or_default() {
            println!("Command response OK! {:02X?}", data);
            self.sync_stub_detected = val == 0;
            for _ in 0..7 {
                match self.command(None, &[], 0, true, DEFAULT_TIMEOUT).unwrap_or_default() {
                    Some((v, _)) => {
                        self.sync_stub_detected &= v == 0
                    },
                    None => {return false;}
                }
            }
        }
        return false;
    }

    fn read(&mut self) -> Option<Vec<u8>> {
        if let Some(reader) = self.slip_reader.as_mut() {
            return reader.queue.try_recv().ok()
        } else {
            eprintln!("Trying to read without SLIP reader!?");
        }
        None
    }

    fn command(&mut self, op: Option<u8>, data: &[u8], chk: u32, wait_response: bool, timeout: u64) -> std::io::Result<Option<(u32, Vec<u8>)>> {
        if let Some(pid) = op {
            self.port.write_all(&((data.len() as u16).to_le_bytes()))?;
            self.port.write_all(&(chk).to_le_bytes())?;
            self.port.write_all(data)?;
        }
        if wait_response == false {
            return Ok(None);
        }

        for retry in 0..100 {
            if let Some(read) = self.read() {
                println!("Read {:02X?}", read);
                if read.len() < 8 {
                    continue;
                }
                let resp = read[0];
                let op_ret = read[1];
                let len_ret = u16::from_le_bytes(read[2..4].try_into().unwrap());
                let val = u32::from_le_bytes(read[4..8].try_into().unwrap());
                if resp != 1 {
                    continue;
                }
                let read_data = &read[8..];
                if op.is_none() || op.unwrap() == op_ret {
                    return Ok(Some((val, read_data.to_vec())))
                }
                if data[0] != 0 && data[1] == ROM_INVALID_RECV_MSG {
                    self.flush_input();
                    return Ok(None);
                }
            } else {
                std::thread::sleep(std::time::Duration::from_millis(50));
            }
        }
        return Ok(None);
    }


    fn set_dtr(&mut self, state: bool) {
        self.port.set_data_terminal_ready(state);
        self.dtr = state;
    }

    fn set_rts(&mut self, state: bool) {
        self.port.set_request_to_send(state);
        self.port.set_data_terminal_ready(self.dtr);
    }
}