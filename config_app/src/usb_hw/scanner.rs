use super::diag_usb::Nag52USB;



pub struct Nag52UsbScanner {}

impl ecu_diagnostics::hardware::HardwareScanner<Nag52USB> for Nag52UsbScanner {
    fn list_devices(&self) -> Vec<ecu_diagnostics::hardware::HardwareInfo> {
        todo!()
    }

    fn open_device_by_index(&self, idx: usize) -> ecu_diagnostics::hardware::HardwareResult<std::sync::Arc<std::sync::Mutex<Nag52USB>>> {
        todo!()
    }

    fn open_device_by_name(&self, name: &str) -> ecu_diagnostics::hardware::HardwareResult<std::sync::Arc<std::sync::Mutex<Nag52USB>>> {
        todo!()
    }
}