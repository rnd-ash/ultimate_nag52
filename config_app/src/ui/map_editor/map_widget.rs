use eframe::{
    egui::{self, Label, Response, Sense, Visuals},
    epaint::{text::LayoutJob, FontId, Fonts, Pos2, Rect, Rounding, Shape, Stroke, TextShape},
};

pub struct MapWidget<'a>(&'a super::Map);

impl<'a> MapWidget<'a> {
    pub fn new(map: &'a super::Map) -> Self {
        Self(map)
    }
}

impl<'a> egui::Widget for MapWidget<'a> {
    fn ui(self, ui: &mut eframe::egui::Ui) -> egui::Response {
        let space_per_column = 100.0;
        let row_height = 20.0;
        let header_column_width = 150.0;
        let padding = 5f32;
        let line_visuals = ui.visuals().text_color();
        let galley_y = ui.painter().layout(
            self.0.meta.y_desc.to_string(),
            FontId::monospace(12.0),
            line_visuals,
            // Y column count
            row_height*(self.0.y_values.len() as f32)
        );
        let galley_y_size = galley_y.size();
        let galley_x = ui.painter().layout_no_wrap(
            self.0.meta.x_desc.to_string(),
            FontId::monospace(12.0),
            line_visuals,
        );
        let galley_x_size = galley_x.size();


        let table_width = (space_per_column * self.0.x_values.len() as f32) + header_column_width;
        let table_height = row_height * self.0.y_values.len() as f32;
        let desired_size = egui::Vec2::new(table_width+galley_y_size.x, table_height+galley_x_size.y);
        let (rect, mut response) = ui.allocate_exact_size(desired_size, egui::Sense::click());
        let painter = ui.painter();
        let mut b = false;


        for col in 0..self.0.y_values.len() {
            let c = match b {
                true => ui.visuals().faint_bg_color,
                false => ui.visuals().window_fill(),
            };
            let idx = col as f32;
            painter.rect(
                Rect::from_two_pos(
                    Pos2::new(rect.left(), rect.top() + (idx * row_height)),
                    Pos2::new(rect.right(), rect.top() + ((idx + 1.0) * row_height)),
                ),
                Rounding::none(),
                c,
                Stroke::none(),
            );

            b = !b;
        }

        // H-line (Heading X values)
        painter.line_segment(
            [
                (rect.left(), row_height + rect.top()).into(),
                (table_width + rect.left(), row_height + rect.top()).into(),
            ],
            Stroke::new(2.0, line_visuals),
        );
        // V-line (Heading V values)

        for i in 0..self.0.x_values.len() {
            // H-line (Heading X values)
            let pos = header_column_width + ((i as f32) * space_per_column);
            painter.line_segment(
                [
                    (rect.left() + pos, rect.top() + row_height).into(),
                    (rect.left() + pos, rect.bottom()).into(),
                ],
                Stroke::new(1.0, line_visuals),
            );
        }

        

        let idx = ui.painter().add(TextShape {
            pos: (rect.left(), rect.bottom()).into(),
            galley: galley_y,
            underline: Stroke::none(),
            override_text_color: None,
            angle: -1.5708,
        });

        painter.line_segment(
            [
                (header_column_width + rect.left(), rect.top()).into(),
                (header_column_width + rect.left(), table_height + rect.top()).into(),
            ],
            Stroke::new(2.0, line_visuals),
        );
        response
    }
}
