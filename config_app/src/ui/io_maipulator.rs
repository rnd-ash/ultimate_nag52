use std::{
    char::MAX,
    sync::{
        atomic::{AtomicBool, AtomicU64, Ordering},
        Arc, Mutex, RwLock,
    },
    thread,
    time::{Duration, Instant},
};

use ecu_diagnostics::kwp2000::{Kwp2000DiagnosticServer, SessionType};
use eframe::egui::plot::{Legend, Line, Plot};

use crate::{ui::status_bar::MainStatusBar, window::PageAction};

use rli::{DataSolenoids, LocalRecordData, RecordIdents};

use super::diagnostics::rli;

const UPDATE_DELAY_MS: u64 = 500;

pub struct IoManipulatorPage {
    bar: MainStatusBar,
    query_ecu: Arc<AtomicBool>,
    curr_solenoid_values: Arc<RwLock<Option<DataSolenoids>>>,
    time_since_launch: Instant,
    show_ui: bool,
}

impl IoManipulatorPage {
    pub fn new(server: Arc<Mutex<Kwp2000DiagnosticServer>>, bar: MainStatusBar) -> Self {
        let run = Arc::new(AtomicBool::new(true));
        let run_t = run.clone();

        let store = Arc::new(RwLock::new(None));
        let store_t = store.clone();

        let store_old = Arc::new(RwLock::new(None));
        let store_old_t = store_old.clone();

        let launch_time = Instant::now();
        let launch_time_t = launch_time.clone();

        let last_update = Arc::new(AtomicU64::new(0));
        let last_update_t = last_update.clone();
        thread::spawn(move || {
            let _ = server
                .lock()
                .unwrap()
                .set_diagnostic_session_mode(SessionType::Normal);
            while run_t.load(Ordering::Relaxed) {
                let start = Instant::now();
                if let Ok(r) = RecordIdents::SolenoidStatus.query_ecu(&mut *server.lock().unwrap())
                {
                    if let LocalRecordData::Solenoids(s) = r {
                        let curr = *store_t.read().unwrap();
                        *store_old_t.write().unwrap() = curr;
                        *store_t.write().unwrap() = Some(s);
                        last_update_t.store(
                            launch_time_t.elapsed().as_millis() as u64,
                            Ordering::Relaxed,
                        );
                    }
                }
                let taken = start.elapsed().as_millis() as u64;
                if taken < UPDATE_DELAY_MS {
                    std::thread::sleep(Duration::from_millis(UPDATE_DELAY_MS - taken));
                }
            }
        });

        Self {
            bar,
            query_ecu: run,
            curr_solenoid_values: store,
            time_since_launch: launch_time,
            show_ui: false,
        }
    }
}

const GRAPH_TIME_MS: u16 = 100;
const MAX_DUTY: u16 = 0xFFF; // 12bit pwm (4096)

const VOLTAGE_HIGH: u16 = 12;
const VOLTAGE_LOW: u16 = 0;

impl crate::window::InterfacePage for IoManipulatorPage {
    fn make_ui(
        &mut self,
        ui: &mut eframe::egui::Ui,
        frame: &eframe::Frame,
    ) -> crate::window::PageAction {
        ui.heading("IO Manipulator");

        ui.label("
            This UI is only intended for debugging the TCM on a test bench. It is to NEVER be used whilst the TCM is inside a vehicle.

            If the TCM detects it is inside a vehicle, it will reject any request.

            Upon exiting this page, the TCM will reboot to reset to its default state.
        ");

        if !self.show_ui {
            let mut btn_action = None;
            ui.horizontal(|row| {
                if row.button("I understand").clicked() {}
                if row.button("Take me to safety").clicked() {
                    btn_action = Some(PageAction::Destroy);
                }
            });
            if let Some(req) = btn_action {
                return req;
            }
        }

        PageAction::None
    }

    fn get_title(&self) -> &'static str {
        "IO Manipulator view"
    }

    fn get_status_bar(&self) -> Option<Box<dyn crate::window::StatusBar>> {
        Some(Box::new(self.bar.clone()))
    }
}

impl Drop for IoManipulatorPage {
    fn drop(&mut self) {}
}
