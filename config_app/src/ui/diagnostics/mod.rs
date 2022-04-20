use std::collections::VecDeque;
use std::collections::hash_map::DefaultHasher;
use std::hash::{Hash, Hasher};
use std::mem::{size_of, transmute};
use std::sync::{Arc, Mutex};
use std::time::{Duration, Instant};
use ecu_diagnostics::hardware::Hardware;
use ecu_diagnostics::kwp2000::{Kwp2000DiagnosticServer, Kwp2000ServerOptions, Kwp2000VoidHandler};
use egui::plot::{Plot, Value, Line, Values, Legend};
use egui::{Ui, RichText, Color32};
use epi::Frame;
use crate::ui::status_bar::MainStatusBar;
use crate::window::{PageAction, StatusBar};

pub mod rli;
pub mod data;
use crate::usb_hw::diag_usb::Nag52USB;
use ecu_diagnostics::{kwp2000::*, bcd_decode};
use crate::ui::diagnostics::rli::{DataCanDump, DataGearboxSensors, DataSolenoids, LocalRecordData, RecordIdents};

use self::rli::ChartData;

pub enum CommandStatus {
    Ok(String),
    Err(String)
}

pub struct DiagnosticsPage{
    nag: Arc<Mutex<Nag52USB>>,
    bar: MainStatusBar,
    server: Kwp2000DiagnosticServer,
    last_req_time: Instant,
    text: CommandStatus,
    record_data: Option<LocalRecordData>,
    record_to_query: Option<RecordIdents>,
    last_query_time: Instant,
    query_loop: bool,
    charting_data: VecDeque<(u128, ChartData)>,
    chart_idx: u128
}

impl DiagnosticsPage {
    pub fn new(nag: Arc<Mutex<Nag52USB>>, bar: MainStatusBar) -> Self {

        let mut channel = Hardware::create_iso_tp_channel(nag.clone()).unwrap();
        let channel_cfg = ecu_diagnostics::channel::IsoTPSettings {
            block_size: 8,
            st_min: 20,
            extended_addressing: false,
            pad_frame: true,
            can_speed: 500_000,
            can_use_ext_addr: false,
        };
        let server_settings = Kwp2000ServerOptions {
            send_id: 0x07E1,
            recv_id: 0x07E9,
            read_timeout_ms: 2000,
            write_timeout_ms: 2000,
            global_tp_id: 0,
            tester_present_interval_ms: 2000,
            tester_present_require_response: true,
            global_session_control: false
        };
        let mut kwp = Kwp2000DiagnosticServer::new_over_iso_tp(
            server_settings,
            channel,
            channel_cfg,
            Kwp2000VoidHandler {},
        ).unwrap();

        Self {
            server: kwp,
            nag,
            bar,
            last_req_time: Instant::now(),
            text: CommandStatus::Ok("".into()),
            record_data: None,
            record_to_query: None,
            last_query_time: Instant::now(),
            query_loop: false,
            charting_data: VecDeque::new(),
            chart_idx: 0
        }
    }
}


impl crate::window::InterfacePage for DiagnosticsPage {
    fn make_ui(&mut self, ui: &mut Ui, frame: &Frame) -> PageAction {
        ui.heading("This is experimental, use with MOST up-to-date firmware");

        if ui.button("Query ECU Serial number").clicked() {
            match self.server.read_ecu_serial_number() {
                Ok(b) => {
                    self.text = CommandStatus::Ok(format!("ECU Serial: {}", String::from_utf8_lossy(&b).to_string()))
                },
                Err(e) => self.text = CommandStatus::Err(e.to_string())
            }
        }

        if ui.button("Query ECU data").clicked() {
            match self.server.read_daimler_identification() {
                Ok(b) => {
                    self.text = CommandStatus::Ok(format!(
                        r#"
                        Part number: {}
                        Software date {}
                        ECU production date: {}
                        "#,
                        b.part_number,
                        b.get_software_date_pretty(),
                        b.get_production_date_pretty()
                    ))
                },
                Err(e) => self.text = CommandStatus::Err(e.to_string())
            }
        }

        if ui.button("Query gearbox sensor").clicked() {
            self.record_to_query = Some(RecordIdents::GearboxSensors);
            self.chart_idx = 0;
            self.charting_data.clear();
            self.record_data = None;
        }
        if ui.button("Query gearbox solenoids").clicked() {
            self.record_to_query = Some(RecordIdents::SolenoidStatus);
            self.chart_idx = 0;
            self.charting_data.clear();
            self.record_data = None;
        }
        if ui.button("Query can Rx data").clicked() {
            self.record_to_query = Some(RecordIdents::CanDataDump);
            self.chart_idx = 0;
            self.charting_data.clear();
            self.record_data = None;
        }

        if ui.button("Query Performance metrics").clicked() {
            self.record_to_query = Some(RecordIdents::SysUsage);
            self.chart_idx = 0;
            self.charting_data.clear();
            self.record_data = None;
        }
        ui.checkbox(&mut self.query_loop, "Loop query data");

        match &self.text {
            CommandStatus::Ok(res) => {
                ui.label(RichText::new(res).color(Color32::from_rgb(0, 255, 0)));
            },
            CommandStatus::Err(res) => {
                ui.label(RichText::new(res).color(Color32::from_rgb(255, 0, 0)));
            },
        }

        if self.query_loop && self.last_query_time.elapsed().as_millis() > 100 {
            self.last_query_time = Instant::now();
            self.chart_idx += 100;
            if let Some(rid) = self.record_to_query {
                if let Ok(r) = rid.query_ecu(&mut self.server) {
                    self.record_data = Some(r)
                }
            }
        }

        if let Some(data) = &self.record_data  {
            data.to_table(ui);

            let c = data.get_chart_data();

            if !c.is_empty() {
                let d = &c[0];
                self.charting_data.push_back((
                    self.chart_idx,
                    d.clone()
                ));

                if self.charting_data.len() > 500 {
                    let _ = self.charting_data.pop_front();
                }

                // Can guarantee everything in `self.charting_data` will have the SAME length
                // as `d`
                let mut lines = Vec::new();
                let mut legend = Legend::default();

                for (idx, (key, _, _)) in d.data.iter().enumerate() {
                    let mut points: Vec<Value> = Vec::new();
                    for (timestamp, point) in &self.charting_data {
                        points.push(Value::new(*timestamp as f64, point.data[idx].1))
                    }
                    let mut key_hasher = DefaultHasher::default();
                    key.hash(&mut key_hasher);
                    let r = key_hasher.finish();
                    lines.push(Line::new(Values::from_values(points)).name(key.clone()).color(Color32::from_rgb((r & 0xFF) as u8, ((r >> 8) & 0xFF) as u8, ((r >> 16) & 0xFF) as u8)))
                }

                let mut plot = Plot::new(d.group_name.clone())
                        .allow_drag(false)
                        .legend(legend);
                if let Some((min, max)) = &d.bounds {
                    plot = plot.include_y(*min);
                    if *max > 0.1 { // 0.0 check
                        plot = plot.include_y(*max);
                    }
                }

                plot.show(ui, |plot_ui| {
                    for x in lines {
                        plot_ui.line(x)
                    }
                });
            }
        }

        PageAction::None
    }

    fn get_title(&self) -> &'static str {
        "Ultimate-NAG52 diagnostics"
    }

    fn get_status_bar(&self) -> Option<Box<dyn StatusBar>> {
        Some(Box::new(self.bar.clone()))
    }
}