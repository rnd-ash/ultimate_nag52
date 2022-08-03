use std::{sync::{Arc, atomic::{AtomicBool, Ordering}, RwLock, Mutex}, thread, time::Duration};

use ecu_diagnostics::{kwp2000::{Kwp2000DiagnosticServer, SessionType, KWP2000Error}, DiagnosticServer};
use eframe::{egui::{plot::{Plot, Points, Line, LinkedAxisGroup, VLine, Text, LineStyle, PlotUi, Corner, Value, Values}, RichText}, epaint::{Stroke, Color32}};
use modular_bitfield::bitfield;
use serde::{Serialize, Deserialize};

use crate::{ui::status_bar::MainStatusBar, window::PageAction};
use crate::ui::egui::ComboBox;

// Data structure for shift report
pub const MAX_POINTS_PER_SR_ARRAY: usize = 6000/50;
pub const REPORT_LEN: usize = std::mem::size_of::<ShiftReport>();

#[repr(packed)]
#[derive(Debug, Clone, Copy, Ord, PartialOrd, Eq, PartialEq, Deserialize, Serialize)]
struct ShiftPhase {
    ramp_time: u16,
    hold_time: u16,
    spc_pressure: u16,
    mpc_pressure: u16
}


#[repr(packed)]
#[derive(Debug, Clone, Copy, Ord, PartialOrd, Eq, PartialEq)]
struct ShiftReport {
    atf_temp_c: i16,
    targ_curr: u8,
    profile: u8,
    requested_torque: u16,
    interval_points: u8,
    report_len: u16,
    engine_rpm: [u16; MAX_POINTS_PER_SR_ARRAY],
    input_rpm: [u16; MAX_POINTS_PER_SR_ARRAY],
    output_rpm: [u16; MAX_POINTS_PER_SR_ARRAY],
    engine_torque: [i16; MAX_POINTS_PER_SR_ARRAY],
    total_ms: u16,
    initial_mpc_pressure: u16,
    hold1_data: ShiftPhase,
    hold2_data: ShiftPhase,
    hold3_data: ShiftPhase,
    torque_data: ShiftPhase,
    overlap_data: ShiftPhase,
    max_pressure_data: ShiftPhase,
    transition_start: u16,
    transition_end: u16,
    flare_timestamp: u16,
    timeout: u8,
}

#[derive(Debug, Clone, Ord, PartialOrd, Eq, PartialEq)]
struct SerializedShiftReport {
    atf_temp_c: i16,
    targ_curr: u8,
    profile: u8,
    requested_torque: u16,
    interval_points: u8,
    report_len: u16,
    engine_rpm: Vec<u16>,
    input_rpm: Vec<u16>,
    output_rpm: Vec<u16>,
    engine_torque: Vec<i16>,
    total_ms: u16,
    initial_mpc_pressure: u16,
    hold1_data: ShiftPhase,
    hold2_data: ShiftPhase,
    hold3_data: ShiftPhase,
    torque_data: ShiftPhase,
    overlap_data: ShiftPhase,
    max_pressure_data: ShiftPhase,
    transition_start: u16,
    transition_end: u16,
    flare_timestamp: u16,
    timeout: u8,
}

impl SerializedShiftReport {
    fn from_raw(rpt: &ShiftReport) -> Self {
        let mut r = Self {
            atf_temp_c: rpt.atf_temp_c,
            targ_curr: rpt.targ_curr,
            profile: rpt.profile,
            requested_torque: rpt.requested_torque,
            interval_points: rpt.interval_points,
            report_len: rpt.report_len,
            engine_rpm: (&{rpt.engine_rpm}).to_vec(),
            input_rpm: (&{rpt.input_rpm}).to_vec(),
            output_rpm: (&{rpt.output_rpm}).to_vec(),
            engine_torque: (&{rpt.engine_torque}).to_vec(),
            total_ms: rpt.total_ms,
            initial_mpc_pressure: rpt.initial_mpc_pressure,
            hold1_data: rpt.hold1_data,
            hold2_data: rpt.hold2_data,
            hold3_data: rpt.hold3_data,
            torque_data: rpt.torque_data,
            overlap_data: rpt.overlap_data,
            max_pressure_data: rpt.max_pressure_data,
            transition_start: rpt.transition_start,
            transition_end: rpt.transition_end,
            flare_timestamp: rpt.flare_timestamp,
            timeout: rpt.timeout,
        };
        r.engine_rpm.resize(r.report_len as usize, 0);
        r.input_rpm.resize(r.report_len as usize, 0);
        r.output_rpm.resize(r.report_len as usize, 0);
        r.engine_torque.resize(r.report_len as usize, 0);
        r
    }

    fn to_raw(&self) -> ShiftReport {
        let mut r = ShiftReport {
            atf_temp_c: self.atf_temp_c,
            targ_curr: self.targ_curr,
            profile: self.targ_curr,
            requested_torque: self.requested_torque,
            interval_points: self.interval_points,
            report_len: self.report_len,
            engine_rpm: [0x00; MAX_POINTS_PER_SR_ARRAY],
            input_rpm: [0x00; MAX_POINTS_PER_SR_ARRAY],
            output_rpm: [0x00; MAX_POINTS_PER_SR_ARRAY],
            engine_torque: [0x00; MAX_POINTS_PER_SR_ARRAY],
            total_ms: self.total_ms,
            initial_mpc_pressure: self.initial_mpc_pressure,
            hold1_data: self.hold1_data,
            hold2_data: self.hold2_data,
            hold3_data: self.hold3_data,
            torque_data: self.torque_data,
            overlap_data: self.overlap_data,
            max_pressure_data: self.max_pressure_data,
            transition_start: self.transition_start,
            transition_end: self.transition_end,
            flare_timestamp: self.flare_timestamp,
            timeout: self.timeout,
        };
        unsafe {
            //r.engine_rpm
            //r.engine_rpm.copy_from_slice(&self.engine_rpm);
            //r.input_rpm.copy_from_slice(&self.input_rpm);
            //r.output_rpm.copy_from_slice(&self.output_rpm);
            //r.engine_torque.copy_from_slice(&self.engine_torque);
        }
        r
    }
}


pub struct ShiftReportPage{
    bar: MainStatusBar,
    curr_report: Option<ShiftReport>,
    server: Arc<Mutex<Kwp2000DiagnosticServer>>,
    report_list: Vec<(u8, i32, u8, u8)>, // ID, ATF Temp, curr, target
    err: Option<String>,
    select_id: u32,
    axis_group: LinkedAxisGroup
}

impl ShiftReportPage {
    pub fn new(server: Arc<Mutex<Kwp2000DiagnosticServer>>, bar: MainStatusBar) -> Self {
        server.lock().unwrap().set_diagnostic_session_mode(SessionType::ExtendedDiagnostics);
        Self {
            bar,
            curr_report: None,
            report_list: Vec::new(),
            err: None,
            server,
            select_id: 0,
            axis_group: LinkedAxisGroup::x()
        }
    }
    pub fn parse_error(&mut self, e: ecu_diagnostics::DiagError) {
        match e {
            ecu_diagnostics::DiagError::ECUError { code, def: _ } => {
                // Conditions not correct actually implies in this mode that car is changing gear
                // so we don't have a lock on the resource
                if code == 0x22 {
                    self.err = Some(format!("Car is currently changing gear, cannot query"))
                } else {
                    self.err = Some(format!("Error ECU rejected the request: {}", e))
                }
            },
            _ => self.err = Some(format!("Error querying ECU: {}", e))
        }
    }
}

impl crate::window::InterfacePage for ShiftReportPage {
    fn make_ui(&mut self, ui: &mut eframe::egui::Ui, frame: &eframe::Frame) -> crate::window::PageAction {
        ui.heading("Shift report history");

        if ui.button("Query a list of available shifts").clicked() {
            let rpt_query = self.server.lock().unwrap().send_byte_array_with_response(&[0x88, 0x00, 0x00]);
            match rpt_query {
                Ok(mut res) => {
                    res.drain(0..1);
                    if res.len() % 4 != 0 {
                        self.err = Some(format!("Incorrect report length!"));
                        return PageAction::None
                    }
                    self.report_list.clear();
                    for chunk in res.chunks(4) {
                        let id = chunk[0];
                        let targ = (chunk[1] & 0xF0) >> 4;
                        let curr = chunk[1] & 0x0F;
                        let atf: i16 = (chunk[2] as i16) << 8 | chunk[3] as i16;
                        if targ != 0 && curr != 0 && atf != 0 {
                            self.report_list.push((id, atf as i32, targ, curr))
                        }
                    }
                },
                Err(e) => self.parse_error(e)
            }
        }

        if self.report_list.is_empty() {
            ui.label("No shift data available");
        } else {
            ComboBox::from_label("Select shift record")
            .width(400.0)
            .selected_text(format!("Shift ##{}", self.select_id))
            .show_ui(ui, |cb_ui| {
                for shift in &self.report_list {
                    cb_ui.selectable_value(&mut self.select_id, shift.0 as u32, format!("Shift ##{}", shift.0));
                }
            });

            if ui.button("Query shift").clicked() {
                let rpt_query = self.server.lock().unwrap().send_byte_array_with_response(&[0x88, 0x01, self.select_id as u8]);
                match rpt_query {
                    Ok(mut res) => {
                        res.drain(0..1);
                        if res.len() != REPORT_LEN {
                            self.err = Some(format!("Incorrect report length. Want {} bytes, got {} bytes!", REPORT_LEN, res.len()));
                            return PageAction::None
                        }

                        unsafe {
                            let rpt_ptr: *const ShiftReport = res.as_ptr() as *const ShiftReport;
                            let mut rpt = *rpt_ptr;
                            rpt.transition_end;
                            rpt.transition_start;
                            self.curr_report = Some(rpt)
                        }
                        let x = self.curr_report.clone().unwrap();
                        println!("{} {} {}", &{x.total_ms}, &{x.transition_start}, &{x.transition_end});
                    },

                    Err(e) => self.parse_error(e)
                }
            }
        }

        if let Some(e) = &self.err {
            ui.label(e);
        }

        if let Some(report) = &self.curr_report {

            ui.heading("Shift stats:");
            ui.label(format!("Gear {} to gear {}", report.targ_curr & 0xF, (report.targ_curr & 0xF0) >> 4));
            ui.label(format!("ATF Temp: {}°C", &{report.atf_temp_c}));

            ui.label(format!("Profile: {}", match report.profile { // Profiles.h
                0 => "Standard",
                1 => "Comfort",
                2 => "Winter",
                3 => "Agility",
                4 => "Manual",
                _ => "Unknown"
            }));


            ui.label(
                if report.timeout == 0 {
                    RichText::new(format!("Shift completed after {} ms", &{report.total_ms}))
                } else {
                    RichText::new(format!("Shift timed out after {} ms!", &{report.total_ms})).color(Color32::from_rgb(255, 0, 0))
                }
            );
            if report.timeout == 0 {
                ui.label(format!("Gear transition period: {} ms", report.transition_end - report.transition_start));
            }


            let time_axis: Vec<u16> = (0..=report.total_ms).step_by(report.interval_points as usize).collect();
            let mut time = 0;
            // Add pressure line (Static)
            let mut pressure_spc_points: Vec<Value> = Vec::new();
            let mut pressure_mpc_points: Vec<Value> = Vec::new();
            let mut engine_rpm_points: Vec<Value> = Vec::new();
            let mut input_rpm_points: Vec<Value> = Vec::new();
            let mut output_rpm_points: Vec<Value> = Vec::new();
            let mut torque_points: Vec<Value> = Vec::new();
            pressure_spc_points.push(Value::new(0, 0)); // Always
            pressure_mpc_points.push(Value::new(0, report.initial_mpc_pressure));

            time += report.hold1_data.ramp_time;
            pressure_spc_points.push(Value::new(time, report.hold1_data.spc_pressure));
            pressure_mpc_points.push(Value::new(time, report.hold1_data.mpc_pressure));
            time += report.hold1_data.hold_time;
            pressure_spc_points.push(Value::new(time, report.hold1_data.spc_pressure));
            pressure_mpc_points.push(Value::new(time, report.hold1_data.mpc_pressure));

            time += report.hold2_data.ramp_time;
            pressure_spc_points.push(Value::new(time, report.hold2_data.spc_pressure));
            pressure_mpc_points.push(Value::new(time, report.hold2_data.mpc_pressure));
            time += report.hold2_data.hold_time;
            pressure_spc_points.push(Value::new(time, report.hold2_data.spc_pressure));
            pressure_mpc_points.push(Value::new(time, report.hold2_data.mpc_pressure));

            time += report.hold3_data.ramp_time;
            pressure_spc_points.push(Value::new(time, report.hold3_data.spc_pressure));
            pressure_mpc_points.push(Value::new(time, report.hold3_data.mpc_pressure));
            time += report.hold3_data.hold_time;
            pressure_spc_points.push(Value::new(time, report.hold3_data.spc_pressure));
            pressure_mpc_points.push(Value::new(time, report.hold3_data.mpc_pressure));

            time += report.torque_data.ramp_time;
            pressure_spc_points.push(Value::new(time, report.torque_data.spc_pressure));
            pressure_mpc_points.push(Value::new(time, report.torque_data.mpc_pressure));
            time += report.torque_data.hold_time;
            pressure_spc_points.push(Value::new(time, report.torque_data.spc_pressure));
            pressure_mpc_points.push(Value::new(time, report.torque_data.mpc_pressure));

            time += report.overlap_data.ramp_time;
            pressure_spc_points.push(Value::new(time, report.overlap_data.spc_pressure));
            pressure_mpc_points.push(Value::new(time, report.overlap_data.mpc_pressure));
            time += report.overlap_data.hold_time;
            pressure_spc_points.push(Value::new(time, report.overlap_data.spc_pressure));
            pressure_mpc_points.push(Value::new(time, report.overlap_data.mpc_pressure));

            time += report.max_pressure_data.ramp_time;
            pressure_spc_points.push(Value::new(time, report.max_pressure_data.spc_pressure));
            pressure_mpc_points.push(Value::new(time, report.max_pressure_data.mpc_pressure));
            time = report.total_ms;
            pressure_spc_points.push(Value::new(time, report.max_pressure_data.spc_pressure));
            pressure_mpc_points.push(Value::new(time, report.max_pressure_data.mpc_pressure));


            let mut rpm_max = *std::cmp::max(unsafe { report.engine_rpm }.iter().max().unwrap(), unsafe { report.input_rpm }.iter().max().unwrap()) as f64;
            rpm_max = std::cmp::max(rpm_max as u16, *unsafe { report.output_rpm }.iter().max().unwrap()) as f64;
            let trq_max = *unsafe { report.engine_torque}.iter().max().unwrap() as f64;


            for x in 0..report.report_len as usize {
                engine_rpm_points.push(Value::new(time_axis[x], report.engine_rpm[x]));
                input_rpm_points.push(Value::new(time_axis[x], report.input_rpm[x]));
                output_rpm_points.push(Value::new(time_axis[x], report.output_rpm[x]));
                torque_points.push(Value::new(time_axis[x], report.engine_torque[x]));
            }

            // Add phase indication lines


            let spc_pressure_line = Line::new(Values::from_values(pressure_spc_points)).name("SPC Pressure (mBar)");
            let mpc_pressure_line = Line::new(Values::from_values(pressure_mpc_points)).name("MPC Pressure (mBar)");
            let engine_line = Line::new(Values::from_values(engine_rpm_points)).name("Engine (RPM)");
            let output_line = Line::new(Values::from_values(output_rpm_points)).name("Output shaft (RPM)");
            let input_line = Line::new(Values::from_values(input_rpm_points)).name("Input shaft (RPM)");
            let torque_line = Line::new(Values::from_values(torque_points)).name("Engine torque (Nm)");
            
            time = 0;
            time += report.hold1_data.hold_time+report.hold1_data.ramp_time;
            let hold1_end_time = time;
            time += report.hold2_data.hold_time+report.hold2_data.ramp_time;
            let hold2_end_time = time;
            time += report.hold3_data.hold_time+report.hold3_data.ramp_time;
            let hold3_end_time = time;

            time += report.torque_data.hold_time+report.torque_data.ramp_time;
            let torque_end_time = time;
            time += report.overlap_data.hold_time+report.overlap_data.ramp_time;
            let overlap_end_time = time;
            time += report.max_pressure_data.hold_time+report.max_pressure_data.ramp_time;
            let max_p_end_time = time;
            // #27ae60
            let phase_colour = Color32::from_rgb(0x27, 0xae, 0x60);
            let ok_colour = Color32::from_rgb(0, 255, 0);
            let timeout_colour = Color32::from_rgb(255, 0, 0);
            let height_per_chart = (ui.available_height()-50.0)/3.0;
            let legand = eframe::egui::plot::Legend::default().position(Corner::RightBottom);
            let timeout = report.timeout != 0;
            let add_shift_regions = |plot_ui: &mut PlotUi| {
                plot_ui.vline(VLine::new(hold1_end_time).style(LineStyle::dashed_loose()).color(phase_colour));
                plot_ui.vline(VLine::new(hold2_end_time).style(LineStyle::dashed_loose()).color(phase_colour));
                plot_ui.vline(VLine::new(hold3_end_time).style(LineStyle::dashed_loose()).color(phase_colour));
                plot_ui.vline(VLine::new(torque_end_time).style(LineStyle::dashed_loose()).color(phase_colour));
                plot_ui.vline(VLine::new(overlap_end_time).style(LineStyle::dashed_loose()).color(phase_colour));
                plot_ui.vline(VLine::new(max_p_end_time).style(LineStyle::dashed_loose()).color(phase_colour));
                plot_ui.vline(VLine::new(report.total_ms).stroke(Stroke::new(2.0, if timeout {timeout_colour} else {ok_colour})));
            };

            let mut plot_pressure = Plot::new("SPC pressure")
                .legend(legand.clone())
                .height(height_per_chart)
                .allow_drag(false)
                .include_x(0)
                .include_x(report.total_ms as f32 * 1.2)
                .include_y(0)
                .include_y(8000)
                .link_axis(self.axis_group.clone())
                .show(ui, |plot_ui| { 
                    plot_ui.line(spc_pressure_line);
                    plot_ui.line(mpc_pressure_line);
                    add_shift_regions(plot_ui);
                    plot_ui.text(Text::new(Value::new((0+hold1_end_time)/2, 7700), "Bleed"));
                    plot_ui.text(Text::new(Value::new((hold1_end_time+hold2_end_time)/2, 7700), "Fill"));
                    plot_ui.text(Text::new(Value::new((hold2_end_time+hold3_end_time)/2, 7700), "Lock"));
                    plot_ui.text(Text::new(Value::new((hold3_end_time+torque_end_time)/2, 7700), "Torque"));
                    plot_ui.text(Text::new(Value::new((torque_end_time+overlap_end_time)/2, 7700), "Overlap"));
                    plot_ui.text(Text::new(Value::new((overlap_end_time+max_p_end_time)/2, 7700), "Max P"));
                });

            let mut plot_rpm = Plot::new("Input/Engine RPM")
                .legend(legand.clone())
                .height(height_per_chart)
                .allow_drag(false)
                .include_x(0)
                .include_x(report.total_ms as f32 * 1.2)
                .include_y(rpm_max * 1.2)
                .link_axis(self.axis_group.clone())
                .show(ui, |plot_ui| { 
                    plot_ui.line(engine_line);
                    plot_ui.line(input_line);
                    plot_ui.line(output_line);
                    // Show the User where the gear shift occurred
                    if report.timeout == 0 {
                        plot_ui.vline(VLine::new(report.transition_start).style(LineStyle::dashed_loose()).color(Color32::from_rgb(255, 192, 203)));
                        plot_ui.vline(VLine::new(report.transition_end).style(LineStyle::dashed_loose()).color(Color32::from_rgb(255, 192, 203)));
                        plot_ui.text(Text::new(Value::new((report.transition_start+report.transition_end)/2, rpm_max * 0.9), "SHIFT"));
                    }
                    add_shift_regions(plot_ui)
                });
                

            let mut plot_torque = Plot::new("Engine torque")
                .legend(legand)
                .height(height_per_chart)
                .allow_drag(false)
                .include_x(0)
                .include_x(report.total_ms as f32 * 1.2)
                .include_y(trq_max * 1.2)
                .link_axis(self.axis_group.clone())
                .show(ui, |plot_ui| { 
                    plot_ui.line(torque_line);
                    add_shift_regions(plot_ui);
                });
        }

        PageAction::None
    }

    fn get_title(&self) -> &'static str {
        "Shift report history"
    }

    fn get_status_bar(&self) -> Option<Box<dyn crate::window::StatusBar>> {
        Some(Box::new(self.bar.clone()))
    }
}