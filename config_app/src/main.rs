use eframe::egui::Vec2;
use ui::launcher::Launcher;
use window::MainWindow;

mod ui;
mod usb_hw;
mod window;

// IMPORTANT. On windows, only the i686-pc-windows-msvc target is supported (Due to limitations with J2534 and D-PDU!
#[cfg(all(target_arch = "x86_64", target_os = "windows"))]
compile_error!("Windows can ONLY be built using the i686-pc-windows-msvc target!");

fn main() {
    let mut app = MainWindow::new();
    let mut native_options = eframe::NativeOptions::default();
    /*
    if let Ok(img) = image::load_from_memory_with_format(TRAY_ICON, ImageFormat::Png) {
        native_options.icon_data = Some(IconData {
            rgba: img.clone().into_bytes(),
            width: img.width(),
            height: img.height(),
        })
    }
    */
    app.add_new_page(Box::new(Launcher::new()));
    native_options.initial_window_size = Some(Vec2::new(1280.0, 720.0));
    eframe::run_native(Box::new(app), native_options)
}
