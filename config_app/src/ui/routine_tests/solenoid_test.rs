use std::{
    char::MAX,
    collections::btree_map::Range,
    ops::RangeInclusive,
    sync::{
        atomic::{AtomicBool, AtomicU64, AtomicU8, Ordering},
        Arc, Mutex, RwLock,
    },
    thread,
    time::{Duration, Instant},
};

use ecu_diagnostics::{
    kwp2000::{Kwp2000DiagnosticServer, SessionType},
    DiagError, DiagServerResult, DiagnosticServer,
};
use eframe::egui::{
    self,
    plot::{Legend, Line, Plot},
    widgets, Color32, RichText,
};

use crate::{ui::status_bar::MainStatusBar, window::PageAction};

pub struct SolenoidTestPage {
    bar: MainStatusBar,
    test_state: Arc<AtomicU8>,
    test_result: Arc<RwLock<Option<TestResultsSolenoid>>>,
    test_status: Arc<RwLock<String>>,
    server: Arc<Mutex<Kwp2000DiagnosticServer>>,
}

const TempCoefficient: f32 = 0.393; // Copper coils and wires

const ResistanceMeasureTemp: f32 = 25.0; // Mercedes tests resistance at 25C

// From Sonnax data
const ResitanceMPC: std::ops::RangeInclusive<f32> = (4.0..=8.0); // 6
const ResitanceSPC: std::ops::RangeInclusive<f32> = (4.0..=8.0); // 6
const ResitanceTCC: std::ops::RangeInclusive<f32> = (2.0..=4.0); // 3

const ResitanceY3: std::ops::RangeInclusive<f32> = (2.5..=6.5); // 4.5
const ResitanceY4: std::ops::RangeInclusive<f32> = (2.5..=6.5); // 4.5
const ResitanceY5: std::ops::RangeInclusive<f32> = (2.5..=6.5); // 4.5

#[repr(packed)]
#[derive(Debug, Clone, Copy)]
pub struct TestResultsSolenoid {
    atf_temp: i16,
    mpc_off_current: u16,
    spc_off_current: u16,
    tcc_off_current: u16,
    y3_off_current: u16,
    y4_off_current: u16,
    y5_off_current: u16,

    vbatt_mpc: u16,
    mpc_on_current: u16,

    vbatt_spc: u16,
    spc_on_current: u16,

    vbatt_tcc: u16,
    tcc_on_current: u16,

    vbatt_y3: u16,
    y3_on_current: u16,

    vbatt_y4: u16,
    y4_on_current: u16,

    vbatt_y5: u16,
    y5_on_current: u16,
}

impl SolenoidTestPage {
    pub fn new(server: Arc<Mutex<Kwp2000DiagnosticServer>>, bar: MainStatusBar) -> Self {
        Self {
            bar,
            server,
            test_state: Arc::new(AtomicU8::new(0)),
            test_result: Arc::new(RwLock::new(None)),
            test_status: Arc::new(RwLock::new(String::new())),
        }
    }
}

fn calc_resistance(current: u16, batt: u16, temp: i16) -> f32 {
    let resistance_now = batt as f32 / current as f32;
    return resistance_now
        + resistance_now * (((ResistanceMeasureTemp - temp as f32) * TempCoefficient) / 100.0);
}

fn make_resistance_text(c_raw: u16, r: f32, range: RangeInclusive<f32>) -> egui::Label {
    if c_raw == 0 {
        return egui::Label::new(RichText::new("FAIL! Open circuit detected!").color(Color32::RED));
    }
    if (c_raw > 3200 && range != ResitanceTCC) {
        return egui::Label::new(
            RichText::new("FAIL! Short circuit detected!").color(Color32::RED),
        );
    }
    let c: Color32;
    let t: String;
    if range.contains(&r) {
        c = Color32::GREEN;
        t = format!(
            "OK! ({:.2}Ω). Acceptable range is {:.2}..{:.2}Ω",
            r,
            range.start(),
            range.end()
        );
    } else {
        c = Color32::RED;
        t = format!(
            "FAIL! ({:.2}Ω). Acceptable range is {:.2}..{:.2}Ω",
            r,
            range.start(),
            range.end()
        );
    }
    egui::Label::new(RichText::new(t).color(c))
}

impl crate::window::InterfacePage for SolenoidTestPage {
    fn make_ui(
        &mut self,
        ui: &mut eframe::egui::Ui,
        frame: &eframe::Frame,
    ) -> crate::window::PageAction {
        ui.heading("Solenoid test");

        ui.label("
            This test will test each solenoid within the transmission, and ensure
            that their resistance is within specification, and that there is no short circuit within the valve
            body.
        ");

        ui.separator();

        ui.label(
            "
            TEST REQUIRMENTS:

            1. Shifter in D or R
            2. Engine off
            3. Not moving
            4. Battery at least 11.5V
        ",
        );

        let state = self.test_state.load(Ordering::Relaxed);
        if state == 1 {
            // Running
            ui.label("Test running...");
        } else {
            if ui.button("Begin test").clicked() {
                let ctx = ui.ctx().clone();
                let s = self.server.clone();
                let str_ref = self.test_status.clone();
                let state_ref = self.test_state.clone();
                let res_ref = self.test_result.clone();
                std::thread::spawn(move || {
                    state_ref.store(1, Ordering::Relaxed);
                    let mut guard = s.lock().unwrap();

                    if let Err(e) =
                        guard.set_diagnostic_session_mode(SessionType::ExtendedDiagnostics)
                    {
                        *str_ref.write().unwrap() =
                            format!("ECU failed to enter extended diagnostic mode: {}", e);
                        state_ref.store(2, Ordering::Relaxed);
                        ctx.request_repaint();
                        return;
                    }
                    if let Err(e) = guard.send_byte_array_with_response(&[0x31, 0xDE]) {
                        let _ = guard.set_diagnostic_session_mode(SessionType::Normal);
                        *str_ref.write().unwrap() = format!("ECU rejected the test: {}", e);
                        state_ref.store(2, Ordering::Relaxed);
                        ctx.request_repaint();
                        return;
                    }

                    loop {
                        match guard.send_byte_array_with_response(&[0x33, 0xDE]) {
                            // Request test results in a loop
                            Ok(res) => {
                                let routine_res_ptr: *const TestResultsSolenoid =
                                    res[2..].as_ptr() as *const TestResultsSolenoid;
                                let routine_res: TestResultsSolenoid = unsafe { *routine_res_ptr };
                                *res_ref.write().unwrap() = Some(routine_res);
                                *str_ref.write().unwrap() = format!("ECU Test Completed!");
                                break;
                            }
                            Err(e) => {
                                let mut failed = true;
                                if let DiagError::ECUError { code, def: _ } = e {
                                    if code == 0x22 {
                                        failed = false;
                                    } // Just waiting for test to finish!
                                }
                                if failed {
                                    *str_ref.write().unwrap() =
                                        format!("Failed to get ECU test results: {}", e);
                                    break;
                                }
                            }
                        }
                        std::thread::sleep(std::time::Duration::from_millis(500));
                    }
                    let _ = guard.set_diagnostic_session_mode(SessionType::Normal);
                    state_ref.store(2, Ordering::Relaxed);
                    ctx.request_repaint();
                });
            }
            if state == 2 {
                ui.label(self.test_status.read().unwrap().as_str());
                if let Some(results) = self.test_result.read().unwrap().clone() {
                    ui.separator();
                    ui.label(format!(
                        "ATF Temp was {} C. Showing results adjusted to {} C",
                        &{ results.atf_temp },
                        ResistanceMeasureTemp
                    ));

                    egui::Grid::new("S").striped(true).show(ui, |g_ui| {
                        g_ui.label("MPC Solenoid");
                        g_ui.add(make_resistance_text(
                            results.mpc_on_current,
                            calc_resistance(
                                results.mpc_on_current,
                                results.vbatt_mpc,
                                results.atf_temp,
                            ),
                            ResitanceMPC,
                        ));
                        g_ui.end_row();

                        g_ui.label("SPC Solenoid");
                        g_ui.add(make_resistance_text(
                            results.spc_on_current,
                            calc_resistance(
                                results.spc_on_current,
                                results.vbatt_spc,
                                results.atf_temp,
                            ),
                            ResitanceSPC,
                        ));
                        g_ui.end_row();

                        g_ui.label("TCC Solenoid");
                        g_ui.add(make_resistance_text(
                            results.tcc_on_current,
                            calc_resistance(
                                results.tcc_on_current,
                                results.vbatt_tcc,
                                results.atf_temp,
                            ),
                            ResitanceTCC,
                        ));
                        g_ui.end_row();

                        g_ui.label("Y3 shift Solenoid");
                        g_ui.add(make_resistance_text(
                            results.y3_on_current,
                            calc_resistance(
                                results.y3_on_current,
                                results.vbatt_y3,
                                results.atf_temp,
                            ),
                            ResitanceY3,
                        ));
                        g_ui.end_row();

                        g_ui.label("Y4 shift Solenoid");
                        g_ui.add(make_resistance_text(
                            results.y4_on_current,
                            calc_resistance(
                                results.y4_on_current,
                                results.vbatt_y4,
                                results.atf_temp,
                            ),
                            ResitanceY4,
                        ));
                        g_ui.end_row();

                        g_ui.label("Y5 shift Solenoid");
                        g_ui.add(make_resistance_text(
                            results.y5_on_current,
                            calc_resistance(
                                results.y5_on_current,
                                results.vbatt_y5,
                                results.atf_temp,
                            ),
                            ResitanceY5,
                        ));
                        g_ui.end_row();
                    });
                }
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
