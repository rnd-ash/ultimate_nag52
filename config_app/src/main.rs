#[macro_use]
extern crate static_assertions;

use std::{iter, env};
use eframe::{NativeOptions, Renderer, IconData, epaint::Vec2};
use ui::launcher::Launcher;
use window::MainWindow;

mod ui;
mod usb_hw;
mod window;

// IMPORTANT. On windows, only the i686-pc-windows-msvc target is supported (Due to limitations with J2534 and D-PDU!
#[cfg(all(target_arch = "x86_64", target_os = "windows"))]
compile_error!("Windows can ONLY be built using the i686-pc-windows-msvc target!");

fn main() {
    env_logger::init();

    let icon = image::load_from_memory(include_bytes!("../logo.png")).unwrap().to_rgba8();
    let (icon_w, icon_h) = icon.dimensions();

    #[cfg(unix)]
    std::env::set_var("WINIT_UNIX_BACKEND", "x11");

    let mut app = window::MainWindow::new();
    app.add_new_page(Box::new(Launcher::new()));
    let mut native_options = NativeOptions::default();
    native_options.icon_data = Some(IconData{
        rgba: icon.into_raw(),
        width: icon_w,
        height: icon_h,
    });
    native_options.initial_window_size = Some(Vec2::new(1280.0, 720.0));
    #[cfg(windows)]
    {
        native_options.renderer = Renderer::Wgpu;
    }
    eframe::run_native("Ultimate NAG52 config suite", native_options, Box::new(|cc| {
        Box::new(app)
    }));
}
