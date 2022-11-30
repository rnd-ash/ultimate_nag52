use std::{
    sync::{
        atomic::{AtomicBool, AtomicU64, Ordering},
        Arc, Mutex, RwLock,
    },
    thread,
    time::{Duration, Instant}, ops::RangeInclusive,
};

use ecu_diagnostics::kwp2000::{Kwp2000DiagnosticServer, SessionType};
use eframe::egui::plot::{Legend, Line, Plot, PlotPoints, Bar, BarChart};

use crate::{ui::status_bar::MainStatusBar, window::PageAction};

use super::rli::{DataSolenoids, LocalRecordData, RecordIdents};

const UPDATE_DELAY_MS: u64 = 100;

pub struct SolenoidPage {
    bar: MainStatusBar,
    query_ecu: Arc<AtomicBool>,
    last_update_time: Arc<AtomicU64>,
    curr_values: Arc<RwLock<Option<DataSolenoids>>>,
    prev_values: Arc<RwLock<Option<DataSolenoids>>>,
    time_since_launch: Instant,
    view_type: ViewType
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum ViewType {
    Pwm,
    Current
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
            curr_values: store,
            last_update_time: last_update,
            prev_values: store_old,
            time_since_launch: launch_time,
            view_type: ViewType::Pwm
        }
    }
}

const GRAPH_TIME_MS: f64 = 100.0;
const MAX_DUTY: u16 = 0xFFF; // 12bit pwm (4096)

const VOLTAGE_HIGH: f64 = 12.0;
const VOLTAGE_LOW: f64 = 0.0;

fn make_pwm_bar(idx: usize, duty: f32) -> Bar {
    return Bar::new(idx as f64, (duty as f64 / 4096.0) * 100.0);
}

fn make_current_bar(idx: usize, c: f32) -> Bar {
    return Bar::new(idx as f64, c as f64);
}

impl crate::window::InterfacePage for SolenoidPage {
    fn make_ui(
        &mut self,
        ui: &mut eframe::egui::Ui,
        frame: &eframe::Frame,
    ) -> crate::window::PageAction {
        ui.heading("Solenoid live view");
        ui.horizontal(|row| {
            row.label("Showing: ");
            row.selectable_value(&mut self.view_type, ViewType::Pwm, "PWM");
            row.selectable_value(&mut self.view_type, ViewType::Current, "Current");
        });

        let curr = self.curr_values.read().unwrap().clone().unwrap_or_default();
        let prev = self.prev_values.read().unwrap().clone().unwrap_or_default();

        let ms_since_update = std::cmp::min(
            UPDATE_DELAY_MS,
            self.time_since_launch.elapsed().as_millis() as u64
                - self.last_update_time.load(Ordering::Relaxed),
        );

        let mut proportion_curr: f32 = (ms_since_update as f32) / UPDATE_DELAY_MS as f32; // Percentage of old value to use
        let mut proportion_prev: f32 = 1.0 - proportion_curr; // Percentage of curr value to use
        if ms_since_update == 0 {
            proportion_prev = 0.5;
            proportion_curr = 0.5;
        } else if ms_since_update == UPDATE_DELAY_MS {
            proportion_prev = 0.5;
            proportion_curr = 0.5;
        }

        let mut bars = Vec::new();
        let mut legend = Legend::default();
        let x_fmt = |y, _range: &RangeInclusive<f64>| {
            match y as usize {
                1 => "MPC",
                2 => "SPC",
                3 => "TCC",
                4 => "Y3",
                5 => "Y4",
                6 => "Y5",
                _ => ""
            }.to_string()
        };

        let mut plot = Plot::new("Solenoid data")
            .allow_drag(false)
            .include_y(0)
            .legend(legend.clone())
            .x_axis_formatter(x_fmt)
            .allow_zoom(false)
            .allow_boxed_zoom(false)
            .allow_scroll(false)
            .allow_drag(false);

        if self.view_type == ViewType::Pwm {
            bars.push(
                make_pwm_bar(1, (curr.mpc_pwm() as f32 * proportion_curr)
                + (prev.mpc_pwm() as f32 * proportion_prev)).name("MPC")
            );
            bars.push(
                make_pwm_bar(2, (curr.spc_pwm() as f32 * proportion_curr)
                + (prev.spc_pwm() as f32 * proportion_prev)).name("SPC")
            );
            bars.push(
                make_pwm_bar(3, (curr.tcc_pwm() as f32 * proportion_curr)
                + (prev.tcc_pwm() as f32 * proportion_prev)).name("TCC")
            );
            bars.push(
                make_pwm_bar(4, (curr.y3_pwm() as f32 * proportion_curr)
                + (prev.y3_pwm() as f32 * proportion_prev)).name("Y3")
            );
            bars.push(
                make_pwm_bar(5, (curr.y4_pwm() as f32 * proportion_curr)
                + (prev.y4_pwm() as f32 * proportion_prev)).name("Y4")
            );
            bars.push(
                make_pwm_bar(6, (curr.y5_pwm() as f32 * proportion_curr)
                + (prev.y5_pwm() as f32 * proportion_prev)).name("Y5")
            );
            let mut y_fmt_pwm = |x, _range: &RangeInclusive<f64>| {
                format!("{} %", x)
            };
            plot = plot.y_axis_formatter( y_fmt_pwm);
            plot = plot.include_y(100);
        } else {
            bars.push(
                make_current_bar(1, (curr.mpc_current() as f32 * proportion_curr)
                + (prev.mpc_current() as f32 * proportion_prev)).name("MPC")
            );
            bars.push(
                make_current_bar(2, (curr.spc_current() as f32 * proportion_curr)
                + (prev.spc_current() as f32 * proportion_prev)).name("SPC")
            );
            bars.push(
                make_current_bar(3, (curr.tcc_current() as f32 * proportion_curr)
                + (prev.tcc_current() as f32 * proportion_prev)).name("TCC")
            );
            bars.push(
                make_current_bar(4, (curr.y3_current() as f32 * proportion_curr)
                + (prev.y3_current() as f32 * proportion_prev)).name("Y3")
            );
            bars.push(
                make_current_bar(5, (curr.y4_current() as f32 * proportion_curr)
                + (prev.y4_current() as f32 * proportion_prev)).name("Y4")
            );
            bars.push(
                make_current_bar(6, (curr.y5_current() as f32 * proportion_curr)
                + (prev.y5_current() as f32 * proportion_prev)).name("Y5")
            );
            let mut y_fmt_current = |x, _range: &RangeInclusive<f64>| {
                format!("{} mA", x)
            };
            plot = plot.y_axis_formatter( y_fmt_current);
            plot = plot.include_y(2000);
        }
        plot.show(ui, |plot_ui| {
            for bar in bars {
                plot_ui.bar_chart(BarChart::new(vec![bar]))
            }
        });
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
