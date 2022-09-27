use std::sync::mpsc;

use ecu_diagnostics::{ServerEventHandler, kwp2000::SessionType, ServerEvent};
use eframe::epaint::Color32;

pub mod diag_usb;
pub mod scanner;
pub mod flasher;

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum KwpEventLevel {
    Info,
    Warn,
    Err
}

pub struct KwpEventHandler {
    evt_queue: mpsc::Sender<(KwpEventLevel, String)>
}

impl KwpEventHandler {
    pub fn new(sender: mpsc::Sender<(KwpEventLevel, String)>) -> Self {
        Self { evt_queue: sender }
    }
}

unsafe impl Sync for KwpEventHandler{}
unsafe impl Send for KwpEventHandler{}

impl ServerEventHandler<SessionType> for KwpEventHandler {
    #[allow(unused_results)]
    fn on_event(&mut self, e: ecu_diagnostics::ServerEvent<SessionType>) {
        match e {
            //ecu_diagnostics::ServerEvent::CriticalError { desc } => todo!(),
            //ecu_diagnostics::ServerEvent::ServerStart => todo!(),
            //ecu_diagnostics::ServerEvent::ServerExit => todo!(),
            ecu_diagnostics::ServerEvent::DiagModeChange { old, new } => {
                self.evt_queue.send((KwpEventLevel::Info, format!("Diag server change mode from '{:?}' to '{:?}'", old, new)));
            },
            ecu_diagnostics::ServerEvent::Request(req) => {
                self.evt_queue.send((KwpEventLevel::Info, format!("OUT {:02X?}", req)));
            },
            ecu_diagnostics::ServerEvent::Response(res) => {
                match res {
                    Ok(payload) => self.evt_queue.send((KwpEventLevel::Info, format!("IN  {:02X?}", payload))),
                    Err(e) => self.evt_queue.send((KwpEventLevel::Err, format!("IN {}", e.to_string())))
                };
            },
            ecu_diagnostics::ServerEvent::TesterPresentError(e) => {
                self.evt_queue.send((KwpEventLevel::Warn, format!("TESTER PRESENT ERROR {}", e)));
            },
            //ecu_diagnostics::ServerEvent::InterfaceCloseOnExitError(_) => todo!(),
            _ => {}
        }
    }
}