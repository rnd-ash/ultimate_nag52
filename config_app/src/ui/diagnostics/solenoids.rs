use std::{sync::{Arc, Mutex, atomic::{AtomicBool, Ordering, AtomicU64}, RwLock}, thread, time::{Duration, Instant}, char::MAX};

use ecu_diagnostics::kwp2000::{Kwp2000DiagnosticServer, SessionType};
use eframe::egui::plot::{Plot, Legend, Line, Values, Value};

use crate::{ui::status_bar::MainStatusBar, window::PageAction};

use super::rli::{DataSolenoids, RecordIdents, LocalRecordData};

const UPDATE_DELAY_MS: u64 = 100;

pub struct SolenoidPage{
    bar: MainStatusBar,
    query_ecu: Arc<AtomicBool>,
    last_update_time: Arc<AtomicU64>,
    curr_values: Arc<RwLock<Option<DataSolenoids>>>,
    prev_values: Arc<RwLock<Option<DataSolenoids>>>,
    time_since_launch: Instant
}

impl SolenoidPage {
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
        thread::spawn(move|| {
            let _ = server.lock().unwrap().set_diagnostic_session_mode(SessionType::Normal);
            while run_t.load(Ordering::Relaxed) {
                let start = Instant::now();
                if let Ok(r) = RecordIdents::SolenoidStatus.query_ecu(&mut *server.lock().unwrap()) {
                    if let LocalRecordData::Solenoids(s) = r {
                        let curr = *store_t.read().unwrap();
                        *store_old_t.write().unwrap() = curr;
                        *store_t.write().unwrap() = Some(s);
                        last_update_t.store(launch_time_t.elapsed().as_millis() as u64, Ordering::Relaxed);
                    }
                }
                let taken = start.elapsed().as_millis() as u64;
                if taken < UPDATE_DELAY_MS {
                    std::thread::sleep(Duration::from_millis(UPDATE_DELAY_MS-taken));
                }
            }
        });



        Self {
            bar,
            query_ecu: run,
            curr_values: store,
            last_update_time: last_update,
            prev_values: store_old,
            time_since_launch: launch_time
        }
    }
}

const GRAPH_TIME_MS: u16 = 100;
const MAX_DUTY: u16 = 0xFFF; // 12bit pwm (4096)

const VOLTAGE_HIGH: u16  = 12;
const VOLTAGE_LOW: u16 = 0;

fn make_line_duty_pwm(duty: f32, freq: u16, x_off: f64, y_off: f64) -> Values {
    let num_pulses = freq / GRAPH_TIME_MS as u16;
    let pulse_width = GRAPH_TIME_MS as f32 / num_pulses as f32;
    let pulse_on_width = (duty as f32/4096f32) * pulse_width;
    let pulse_off_width = pulse_width - pulse_on_width;

    let mut points: Vec<Value> = Vec::new();
    let mut curr_x_pos = 0f32;

    // Shortcut
    if duty as u16 == MAX_DUTY {
        points.push(Value::new(0, VOLTAGE_LOW));
        points.push(Value::new(GRAPH_TIME_MS, VOLTAGE_LOW));
    } else if duty as u16 == 0 {
        points.push(Value::new(0, VOLTAGE_HIGH));
        points.push(Value::new(GRAPH_TIME_MS, VOLTAGE_HIGH));
    } else {
        for i in 0..num_pulses {
            // Start at 12V (High - Solenoid off)
            points.push(Value::new(curr_x_pos, VOLTAGE_HIGH)); // High, left
            curr_x_pos += pulse_off_width;
            points.push(Value::new(curr_x_pos, VOLTAGE_HIGH)); // High, right
            // Now vertical line
            points.push(Value::new(curr_x_pos, VOLTAGE_LOW));
            curr_x_pos += pulse_on_width;
            points.push(Value::new(curr_x_pos, VOLTAGE_LOW));
            // Now draw at 0V (Low - Solenoid on)
        }
    }
    for p in points.iter_mut() {
        p.x += x_off;
        p.y += y_off;
    }
    Values::from_values(points)
}


impl crate::window::InterfacePage for SolenoidPage {


    fn make_ui(&mut self, ui: &mut eframe::egui::Ui, frame: &eframe::Frame) -> crate::window::PageAction {
        ui.heading("Solenoid live view");

        let mut curr = self.curr_values.read().unwrap().clone().unwrap_or_default();
        let mut prev = self.prev_values.read().unwrap().clone().unwrap_or_default();

        let ms_since_update = std::cmp::min(UPDATE_DELAY_MS, self.time_since_launch.elapsed().as_millis() as u64 - self.last_update_time.load(Ordering::Relaxed));

        let mut proportion_curr: f32 = (ms_since_update as f32)/UPDATE_DELAY_MS as f32; // Percentage of old value to use
        let mut proportion_prev: f32 = 1.0 - proportion_curr; // Percentage of curr value to use

        if ms_since_update == 0 {
            proportion_prev = 0.5;
            proportion_curr = 0.5;
        } else if ms_since_update == UPDATE_DELAY_MS {
            proportion_prev = 0.5;
            proportion_curr = 0.5;
        }
        let mut lines = Vec::new();
        let mut legend = Legend::default();
        let c_height = (ui.available_height()-50.0)/6.0;

        lines.push(("MPC", Line::new(make_line_duty_pwm((curr.mpc_pwm() as f32 * proportion_curr) + (prev.mpc_pwm() as f32 * proportion_prev), 1000, 0.0, 0.0)).name("MPC").width(2.0)));
        lines.push(("SPC", Line::new(make_line_duty_pwm((curr.spc_pwm() as f32 * proportion_curr) + (prev.spc_pwm() as f32 * proportion_prev), 1000, 0.0, 0.0)).name("SPC").width(2.0)));
        lines.push(("TCC", Line::new(make_line_duty_pwm((curr.tcc_pwm() as f32 * proportion_curr) + (prev.tcc_pwm() as f32 * proportion_prev), 1000, 0.0, 0.0)).name("TCC").width(2.0)));

        lines.push(("Y3", Line::new(make_line_duty_pwm((curr.y3_pwm() as f32 * proportion_curr) + (prev.y3_pwm() as f32 * proportion_prev), 1000, 0.0, 0.0)).name("Y3").width(2.0)));
        lines.push(("Y4", Line::new(make_line_duty_pwm((curr.y4_pwm() as f32 * proportion_curr) + (prev.y4_pwm() as f32 * proportion_prev), 1000, 0.0, 0.0)).name("Y4").width(2.0)));
        lines.push(("Y5", Line::new(make_line_duty_pwm((curr.y5_pwm() as f32 * proportion_curr) + (prev.y5_pwm() as f32 * proportion_prev), 1000, 0.0, 0.0)).name("Y5").width(2.0)));

        for line in lines {
            let mut plot = Plot::new(format!("Solenoid {}", line.0))
                .allow_drag(false)
                .height(c_height)
                .legend(legend.clone());
            plot = plot.include_y(13);
            plot = plot.include_y(-1);

            plot = plot.include_x(0);
            plot = plot.include_x(100);
            plot.show(ui, |plot_ui| {
                plot_ui.line(line.1)
            });
        }
        ui.ctx().request_repaint();
        PageAction::None
    }

    fn get_title(&self) -> &'static str {
        "Solenoid view"
    }

    fn get_status_bar(&self) -> Option<Box<dyn crate::window::StatusBar>> {
        Some(Box::new(self.bar.clone()))
    }
}

impl Drop for SolenoidPage {
    fn drop(&mut self) {
        self.query_ecu.store(false, Ordering::Relaxed);
    }
}
