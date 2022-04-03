use std::iter;

use egui::FontDefinitions;
use epi::App;
use ui::launcher::Launcher;
use window::MainWindow;
use egui_wgpu_backend::{RenderPass, ScreenDescriptor};
use egui_winit_platform::{Platform, PlatformDescriptor};
use winit::event::Event::*;
use winit::event_loop::ControlFlow;

mod ui;
mod usb_hw;
mod window;

// IMPORTANT. On windows, only the i686-pc-windows-msvc target is supported (Due to limitations with J2534 and D-PDU!
#[cfg(all(target_arch = "x86_64", target_os = "windows"))]
compile_error!("Windows can ONLY be built using the i686-pc-windows-msvc target!");

enum Event {
    RequestRedraw
}

struct RepaintSignal(std::sync::Mutex<winit::event_loop::EventLoopProxy<Event>>);

impl epi::backend::RepaintSignal for RepaintSignal {
    fn request_repaint(&self) {
        self.0.lock().unwrap().send_event(Event::RequestRedraw).ok();
    }
}

fn main() {

    let event_loop = winit::event_loop::EventLoop::with_user_event();
    let window = winit::window::WindowBuilder::new()
        .with_decorations(true)
        .with_resizable(true)
        .with_title("Ultimate-NAG52 configuration utility")
        .with_inner_size(winit::dpi::PhysicalSize {
            width: 1280u32,
            height: 720u32
        })
        .build(&event_loop)
        .unwrap();
    
    #[cfg(windows)]
    let instance = wgpu::Instance::new(wgpu::Backends::DX12 | wgpu::Backends::DX11 | wgpu::Backends::GL);
    #[cfg(unix)]
    let instance = wgpu::Instance::new(wgpu::Backends::PRIMARY);
    let surface = unsafe { instance.create_surface(&window) };

    let adapter = pollster::block_on(instance.request_adapter(&wgpu::RequestAdapterOptions {
        power_preference: wgpu::PowerPreference::LowPower,
        compatible_surface: Some(&surface),
        force_fallback_adapter: false,
    }))
    .unwrap();

    let (device, queue) = pollster::block_on(adapter.request_device(
        &wgpu::DeviceDescriptor {
            features: wgpu::Features::default(),
            limits: wgpu::Limits::default(),
            label: None,
        },
        None,
    ))
    .unwrap();

    let size = window.inner_size();
    let surface_format = surface.get_preferred_format(&adapter).unwrap();
    let mut surface_config = wgpu::SurfaceConfiguration {
        usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
        format: surface_format,
        width: size.width as u32,
        height: size.height as u32,
        present_mode: wgpu::PresentMode::Fifo,
    };
    surface.configure(&device, &surface_config);

    let repaint_signal = std::sync::Arc::new(RepaintSignal(std::sync::Mutex::new(
        event_loop.create_proxy(),
    )));

    // We use the egui_winit_platform crate as the platform.
    let mut platform = Platform::new(PlatformDescriptor {
        physical_width: size.width as u32,
        physical_height: size.height as u32,
        scale_factor: window.scale_factor(),
        font_definitions: FontDefinitions::default(),
        style: Default::default(),
    });

    let mut egui_rpass = RenderPass::new(&device, surface_format, 1);
    let mut state = egui_winit::State::new(4096, &window);
    let context = egui::Context::default();
    let mut app = MainWindow::new();
    app.add_new_page(Box::new(Launcher::new()));
    event_loop.run(move|event, _, control_flow| {
        match event {
            RedrawRequested(..) => {
                let output_frame = match surface.get_current_texture() {
                    Ok(frame) => frame,
                    Err(wgpu::SurfaceError::Outdated) => {
                        // This error occurs when the app is minimized on Windows.
                        // Silently return here to prevent spamming the console with:
                        // "The underlying surface has changed, and therefore the swap chain must be updated"
                        return;
                    }
                    Err(e) => {
                        eprintln!("Dropped frame with error: {}", e);
                        return;
                    }
                };
                let output_view = output_frame
                    .texture
                    .create_view(&wgpu::TextureViewDescriptor::default());

                // Begin to draw the UI frame.
                let input = state.take_egui_input(&window);
                context.begin_frame(input);
                let app_output = epi::backend::AppOutput::default();

                let frame =  epi::Frame::new(epi::backend::FrameData {
                    info: epi::IntegrationInfo {
                        name: "egui_example",
                        web_info: None,
                        cpu_usage: None,
                        native_pixels_per_point: Some(window.scale_factor() as _),
                        prefer_dark_mode: None,
                    },
                    output: app_output,
                    repaint_signal: repaint_signal.clone(),
                });

                // Draw the demo application.
                app.update(&context, &frame);

                // End the UI frame. We could now handle the output and draw the UI with the backend.
                let output = context.end_frame();
                let paint_jobs = context.tessellate(output.shapes);

                let mut encoder = device.create_command_encoder(&wgpu::CommandEncoderDescriptor {
                    label: Some("encoder"),
                });

                // Upload all resources for the GPU.
                let screen_descriptor = ScreenDescriptor {
                    physical_width: surface_config.width,
                    physical_height: surface_config.height,
                    scale_factor: window.scale_factor() as f32,
                };

                egui_rpass.add_textures(&device, &queue, &output.textures_delta).unwrap();
                egui_rpass.remove_textures(output.textures_delta).unwrap();
                egui_rpass.update_buffers(&device, &queue, &paint_jobs, &screen_descriptor);

                // Record all render passes.
                egui_rpass
                    .execute(
                        &mut encoder,
                        &output_view,
                        &paint_jobs,
                        &screen_descriptor,
                        Some(wgpu::Color::BLACK),
                    )
                    .unwrap();
                // Submit the commands.
                queue.submit(iter::once(encoder.finish()));

                // Redraw egui
                output_frame.present();

                // Suppport reactive on windows only, but not on linux.
                // if _output.needs_repaint {
                //     *control_flow = ControlFlow::Poll;
                // } else {
                //     *control_flow = ControlFlow::Wait;
                // }
            }
            MainEventsCleared | UserEvent(Event::RequestRedraw) => {
                window.request_redraw();
            }
            WindowEvent { event, .. } => match event {
                winit::event::WindowEvent::Resized(size) => {
                    // Resize with 0 width and height is used by winit to signal a minimize event on Windows.
                    // See: https://github.com/rust-windowing/winit/issues/208
                    // This solves an issue where the app would panic when minimizing on Windows.
                    if size.width > 0 && size.height > 0 {
                        surface_config.width = size.width;
                        surface_config.height = size.height;
                        surface.configure(&device, &surface_config);
                    }
                }
                winit::event::WindowEvent::CloseRequested => {
                    *control_flow = ControlFlow::Exit;
                }
                event => {
                    // Pass the winit events to the platform integration.
                    state.on_event(&context, &event);
                }
            },
            _ => (),
        }
    });


    /*
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
    */
}
