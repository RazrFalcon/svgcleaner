// svgcleaner could help you to clean up your SVG files
// from unnecessary data.
// Copyright (C) 2012-2017 Evgeniy Reizner
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

use svgdom::{
    AttributeValue,
    Document,
    Node,
};
use svgdom::types::{
    path,
    FuzzyEq,
    Length,
};

use task::short::{EId, AId, Unit};

// TODO: convert thin rect to line-to path
// view-calendar-list.svg

// We should run it after removing invalid or invisible nodes.
//
// We should run it before path processing.
pub fn convert_shapes_to_paths(doc: &Document) {
    for (id, mut node) in doc.descendants().svg() {
        match id {
            EId::Line => convert_line(&mut node),
            EId::Rect => convert_rect(&mut node),
            EId::Polyline => convert_polyline(&mut node),
            EId::Polygon => convert_polygon(&mut node),
            _ => {}
        }
    }
}

fn convert_line(node: &mut Node) {
    debug_assert!(node.is_tag_name(EId::Line));

    let path = {
        let attrs = node.attributes();

        let x1 = get_value!(attrs, Length, AId::X1, Length::zero());
        let y1 = get_value!(attrs, Length, AId::Y1, Length::zero());
        let x2 = get_value!(attrs, Length, AId::X2, Length::zero());
        let y2 = get_value!(attrs, Length, AId::Y2, Length::zero());

        // We can't convert line with non-pixel coordinates.
        // Unit will newer be Px, since we disable it globally via ParseOptions.
        if !(x1.unit == Unit::None && y1.unit == Unit::None &&
             x2.unit == Unit::None && y2.unit == Unit::None) {
            return;
        }

        path::Builder::new()
            .move_to(x1.num, y1.num)
            .line_to(x2.num, y2.num)
            .finalize()
    };

    node.set_attribute((AId::D, path));
    node.set_tag_name(EId::Path);
    node.remove_attributes(&[AId::X1, AId::Y1, AId::X2, AId::Y2]);
}

fn convert_rect(node: &mut Node) {
    debug_assert!(node.is_tag_name(EId::Rect));

    let path = {
        let attrs = node.attributes();

        let rx = get_value!(attrs, Length, AId::Rx, Length::zero());
        let ry = get_value!(attrs, Length, AId::Ry, Length::zero());

        // We converting only simple rects, not rounded,
        // because their path will be longer.
        if !rx.num.is_fuzzy_zero() || !ry.num.is_fuzzy_zero() {
            return;
        }

        let w = get_value!(attrs, Length, AId::Width, Length::zero());
        let h = get_value!(attrs, Length, AId::Height, Length::zero());

        // If values equals to zero than the rect is invisible. Skip it.
        if w.num.is_fuzzy_zero() || h.num.is_fuzzy_zero() {
            return;
        }

        let x = get_value!(attrs, Length, AId::X, Length::zero());
        let y = get_value!(attrs, Length, AId::Y, Length::zero());

        if !(x.unit == Unit::None && y.unit == Unit::None &&
             w.unit == Unit::None && h.unit == Unit::None) {
            return;
        }

        path::Builder::new()
            .move_to(x.num, y.num)
            .hline_to(x.num + w.num)
            .vline_to(y.num + h.num)
            .hline_to(x.num)
            .close_path()
            .finalize()
    };

    node.set_attribute((AId::D, path));
    node.set_tag_name(EId::Path);
    node.remove_attributes(&[AId::X, AId::Y, AId::Rx, AId::Ry, AId::Width, AId::Height]);
}

fn convert_polyline(node: &mut Node) {
    debug_assert!(node.is_tag_name(EId::Polyline));

    let path = match points_to_path(node) {
        Some(p) => p,
        None => return,
    };

    node.set_tag_name(EId::Path);
    node.set_attribute((AId::D, path));
    node.remove_attribute(AId::Points);
}

fn convert_polygon(node: &mut Node) {
    debug_assert!(node.is_tag_name(EId::Polygon));

    let mut path = match points_to_path(node) {
        Some(p) => p,
        None => return,
    };

    path.d.push(path::Segment::new_close_path());

    node.set_tag_name(EId::Path);
    node.set_attribute((AId::D, path));
    node.remove_attribute(AId::Points);
}

fn points_to_path(node: &Node) -> Option<path::Path> {
    let mut path = path::Path::new();

    let attrs = node.attributes();

    let points = if let Some(&AttributeValue::NumberList(ref v)) = attrs.get_value(AId::Points) {
        v
    } else {
        return None;
    };

    // Points with an odd count of coordinates must be fixed in fix_attrs::fix_poly.
    debug_assert_eq!(points.len() % 2, 0);

    let mut i = 0;
    while i < points.len() {
        let seg = if i == 0 {
            path::Segment::new_move_to(points[i], points[i + 1])
        } else {
            path::Segment::new_line_to(points[i], points[i + 1])
        };
        path.d.push(seg);

        i += 2;
    }

    Some(path)
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, ToStringWithOptions};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, convert_shapes_to_paths, $in_text, $out_text);
        )
    }

    test!(conv_line_1,
"<svg>
    <line x1='100' y1='200' x2='300' y2='400'/>
</svg>",
"<svg>
    <path d='M 100 200 L 300 400'/>
</svg>
");

    test!(conv_line_2,
"<svg>
    <line x2='30'/>
</svg>",
"<svg>
    <path d='M 0 0 L 30 0'/>
</svg>
");

    // Ignore, because of non-px unit.
    test!(conv_line_3,
"<svg>
    <line x2='30pt'/>
</svg>",
"<svg>
    <line x2='30pt'/>
</svg>
");

    test!(conv_rect_1,
"<svg>
    <rect x='10' y='20' width='30' height='40'/>
</svg>",
"<svg>
    <path d='M 10 20 H 40 V 60 H 10 Z'/>
</svg>
");

    test!(conv_rect_2,
"<svg>
    <rect width='30' height='40'/>
</svg>",
"<svg>
    <path d='M 0 0 H 30 V 40 H 0 Z'/>
</svg>
");
    // Ignore invalid rects.
    test!(conv_rect_3,
"<svg>
    <rect width='30' height='30' rx='1'/>
    <rect width='30' height='30' ry='1'/>
    <rect width='30' height='0'/>
    <rect width='0' height='30'/>
</svg>",
"<svg>
    <rect height='30' rx='1' width='30'/>
    <rect height='30' ry='1' width='30'/>
    <rect height='0' width='30'/>
    <rect height='30' width='0'/>
</svg>
");

    test!(conv_polyline_1,
"<svg>
    <polyline points='30 40 50 60 70 80'/>
</svg>",
"<svg>
    <path d='M 30 40 L 50 60 L 70 80'/>
</svg>
");

    test!(conv_polyline_2,
"<svg>
    <polyline/>
</svg>",
"<svg>
    <polyline/>
</svg>
");

    test!(conv_polygon_1,
"<svg>
    <polygon points='30 40 50 60 70 80'/>
</svg>",
"<svg>
    <path d='M 30 40 L 50 60 L 70 80 Z'/>
</svg>
");
}
