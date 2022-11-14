use std::{fmt::Display, ops::RangeInclusive};

use eframe::{
    egui,
    emath::{Align2, Pos2, Rect},
    epaint::{Color32, FontId, Rounding},
};

pub fn range_display<T: Into<f32> + Copy + Display>(
    ui: &mut egui::Ui,
    curr_value: T,
    min_ok: T,
    max_ok: T,
    min_display: T,
    max_display: T,
) -> egui::Response {
    let desired_size = egui::Vec2::new(ui.available_width(), 40.0);
    let (rect, mut response) = ui.allocate_exact_size(desired_size, egui::Sense::click());

    let bar_region = Rect::from_two_pos(
        Pos2::new(rect.left(), rect.bottom() - 10.0),
        Pos2::new(rect.right(), rect.top()),
    );

    let total_size_per_range = desired_size.x / (max_display.into() - min_display.into());
    let ok_region_rect = Rect::from_two_pos(
        Pos2::new(
            bar_region.left() + (total_size_per_range * (min_ok.into() - min_display.into())),
            bar_region.bottom(),
        ),
        Pos2::new(
            bar_region.left()
                + (total_size_per_range * (min_ok.into() - min_display.into()))
                + total_size_per_range * (max_ok.into() - min_ok.into()),
            rect.top(),
        ),
    );

    let value_region = Rect::from_two_pos(
        Pos2::new(
            bar_region.left() + (total_size_per_range * (curr_value.into() - min_display.into())),
            bar_region.bottom(),
        ),
        Pos2::new(
            bar_region.left()
                + (total_size_per_range * (curr_value.into() - min_display.into()))
                + 2.0,
            rect.top(),
        ),
    );

    // OK range
    let visuals = ui.style().noninteractive().clone();
    let painter = ui.painter();

    painter.rect(
        bar_region,
        Rounding::from(4.0),
        Color32::RED,
        visuals.bg_stroke,
    );
    painter.rect(
        ok_region_rect,
        Rounding::none(),
        Color32::GREEN,
        visuals.fg_stroke,
    );
    painter.rect(
        value_region,
        Rounding::none(),
        Color32::BLACK,
        visuals.fg_stroke,
    );

    painter.text(
        Pos2::new(rect.left(), rect.bottom() + 5.0),
        Align2::LEFT_BOTTOM,
        format!("{:.2}", min_display),
        FontId::default(),
        visuals.text_color(),
    );
    painter.text(
        Pos2::new(rect.right(), rect.bottom() + 5.0),
        Align2::RIGHT_BOTTOM,
        format!("{:.2}", max_display),
        FontId::default(),
        visuals.text_color(),
    );

    painter.text(
        Pos2::new(ok_region_rect.left(), rect.bottom() + 5.0),
        Align2::CENTER_BOTTOM,
        format!("{:.2}", min_ok),
        FontId::default(),
        visuals.text_color(),
    );
    painter.text(
        Pos2::new(ok_region_rect.right(), rect.bottom() + 5.0),
        Align2::CENTER_BOTTOM,
        format!("{:.2}", max_ok),
        FontId::default(),
        visuals.text_color(),
    );

    painter.text(
        Pos2::new(value_region.right(), rect.bottom() + 5.0),
        Align2::CENTER_BOTTOM,
        format!("{:.2}", curr_value),
        FontId::default(),
        visuals.text_color(),
    );

    response
}
