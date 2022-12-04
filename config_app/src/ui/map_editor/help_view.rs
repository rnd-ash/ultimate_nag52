use eframe::egui;


pub trait HelpView: Clone + Sized {
    fn gen_ui(&self, meta: &super::MapData, raw_ui: &mut egui::Ui) -> bool where Self: Sized;
}


#[derive(Clone, Copy)]
pub struct FillingPressureHelp;

impl HelpView for FillingPressureHelp {
    fn gen_ui(&self, meta: &super::MapData, raw_ui: &mut egui::Ui) -> bool {
        if raw_ui.button("Back").clicked() {
            return true;
        }
        return false;
    }
}