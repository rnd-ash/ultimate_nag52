use std::{sync::{Arc, Mutex, atomic::{AtomicBool, Ordering}, RwLock}, thread, time::Duration, char::MAX};

use ecu_diagnostics::kwp2000::{Kwp2000DiagnosticServer, SessionType};
use egui::plot::{Plot, Legend, Line, Values, Value};

use crate::{ui::status_bar::MainStatusBar, window::PageAction};

use super::rli::{DataSolenoids, RecordIdents, LocalRecordData};


pub struct SolenoidPage{
    bar: MainStatusBar,
    query_ecu: Arc<AtomicBool>,
    curr_values: Arc<RwLock<Option<DataSolenoids>>>
}

impl SolenoidPage {
    pub fn new(server: Arc<Mutex<Kwp2000DiagnosticServer>>, bar: MainStatusBar) -> Self {
        let run = Arc::new(AtomicBool::new(true));
        let run_t = run.clone();

        let store = Arc::new(RwLock::new(None));
        let store_t = store.clone();

        thread::spawn(move|| {
            let _ = server.lock().unwrap().set_diagnostic_session_mode(SessionType::Normal);
            while run_t.load(Ordering::Relaxed) {
                if let Ok(r) = RecordIdents::SolenoidStatus.query_ecu(&mut *server.lock().unwrap()) {
                    if let LocalRecordData::Solenoids(s) = r {
                        *store_t.write().unwrap() = Some(s);
                    }
                }
                std::thread::sleep(Duration::from_millis(100));
            }
        });

        Self {
            bar,
            query_ecu: run,
            curr_values: store
        }
    }
}

const GRAPH_TIME_MS: u16 = 100;
const MAX_DUTY: u16 = 0xFFF; // 12bit pwm (4096)

const VOLTAGE_HIGH: u16  = 12;
const VOLTAGE_LOW: u16 = 0;

fn make_line_duty_pwm(duty: u16, freq: u16, x_off: f64, y_off: f64) -> Values {
    let num_pulses = freq / GRAPH_TIME_MS as u16;
    let pulse_width = GRAPH_TIME_MS as f32 / num_pulses as f32;
    let pulse_on_width = (duty as f32/4096f32) * pulse_width;
    let pulse_off_width = pulse_width - pulse_on_width;

    let mut points: Vec<Value> = Vec::new();
    let mut curr_x_pos = 0f32;

    // Shortcut
    if duty == MAX_DUTY {
        points.push(Value::new(0, VOLTAGE_LOW));
        points.push(Value::new(GRAPH_TIME_MS, VOLTAGE_LOW));
    } else if duty == 0 {
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


    fn make_ui(&mut self, ui: &mut egui::Ui, frame: &epi::Frame) -> crate::window::PageAction {
        ui.heading("Solenoid live view");

        let curr = self.curr_values.read().unwrap().clone().unwrap_or_default();
        let mut lines = Vec::new();
        let mut legend = Legend::default();

        let c_height = ui.available_height()/6.0;

        lines.push(("MPC", Line::new(make_line_duty_pwm(curr.mpc_pwm(), 1000, 0.0, 0.0)).name("MPC").width(2.0)));
        lines.push(("SPC", Line::new(make_line_duty_pwm(curr.spc_pwm(), 1000, 0.0, 0.0)).name("SPC").width(2.0)));
        lines.push(("TCC", Line::new(make_line_duty_pwm(curr.tcc_pwm(), 100, 0.0, 0.0)).name("TCC").width(2.0)));

        lines.push(("Y3", Line::new(make_line_duty_pwm(curr.y3_pwm(), 1000, 0.0, 0.0)).name("Y3").width(2.0)));
        lines.push(("Y4", Line::new(make_line_duty_pwm(curr.y4_pwm(), 1000, 0.0, 0.0)).name("Y4").width(2.0)));
        lines.push(("Y5", Line::new(make_line_duty_pwm(curr.y5_pwm(), 1000, 0.0, 0.0)).name("Y5").width(2.0)));

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