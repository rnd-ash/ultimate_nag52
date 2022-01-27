use std::sync::{Arc, Mutex};

use ecu_diagnostics::{
    hardware::{HardwareCapabilities, HardwareInfo},
    HardwareError,
};

use super::diag_usb::Nag52USB;

pub struct Nag52UsbScanner {
    ports: Vec<HardwareInfo>,
}

impl Nag52UsbScanner {
    pub fn new() -> Self {
        Self {
            ports: match serialport::available_ports() {
                Ok(ports) => ports
                    .iter()
                    .map(|p| HardwareInfo {
                        name: format!("{}", p.port_name),
                        vendor: None,
                        device_fw_version: None,
                        api_version: None,
                        library_version: None,
                        library_location: None,
                        capabilities: HardwareCapabilities {
                            iso_tp: true,
                            can: false,
                            kline: false,
                            kline_kwp: false,
                            sae_j1850: false,
                            sci: false,
                            ip: false,
                        },
                    })
                    .collect::<Vec<HardwareInfo>>(),
                Err(_) => Vec::new(),
            },
        }
    }
}

impl ecu_diagnostics::hardware::HardwareScanner<Nag52USB> for Nag52UsbScanner {
    fn list_devices(&self) -> Vec<ecu_diagnostics::hardware::HardwareInfo> {
        self.ports.clone()
    }

    fn open_device_by_index(
        &self,
        idx: usize,
    ) -> ecu_diagnostics::hardware::HardwareResult<std::sync::Arc<std::sync::Mutex<Nag52USB>>> {
        match self.ports.get(idx) {
            Some(p) => Ok(Arc::new(Mutex::new(Nag52USB::new(&p.name)?))),
            None => Err(HardwareError::DeviceNotFound),
        }
    }

    fn open_device_by_name(
        &self,
        name: &str,
    ) -> ecu_diagnostics::hardware::HardwareResult<std::sync::Arc<std::sync::Mutex<Nag52USB>>> {
        match self.ports.iter().find(|s| s.name == name) {
            Some(p) => Ok(Arc::new(Mutex::new(Nag52USB::new(&p.name)?))),
            None => Err(HardwareError::DeviceNotFound),
        }
    }
}
