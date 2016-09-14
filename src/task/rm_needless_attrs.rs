/****************************************************************************
**
** svgcleaner could help you to clean up your SVG files
** from unnecessary data.
** Copyright (C) 2012-2016 Evgeniy Reizner
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**
****************************************************************************/

use super::short::{EId, AId};

use svgdom::{Document, Node, Attribute};

pub fn remove_needless_attributes(doc: &Document) {
    for node in doc.descendants() {
        match node.tag_id().unwrap() {
            EId::ClipPath => process_clip_path(&node),
            EId::Rect => process_rect(&node),
            EId::Circle => process_circle(&node),
            EId::Ellipse => process_ellipse(&node),
            EId::Line => process_line(&node),
            EId::Polyline | EId::Polygon => process_poly(&node),
            _ => {}
        }
    }
}

fn process_clip_path(node: &Node) {
    for child in node.children() {
        if child.is_used() {
            continue;
        }

        // We can remove any fill and stroke based attrbiutes
        // since they does not impact rendering.
        // https://www.w3.org/TR/SVG/masking.html#EstablishingANewClippingPath
        child.attributes_mut().retain(|a| {
            !(a.is_fill() || a.is_stroke())
        });
    }
}

fn process_rect(node: &Node) {
    // remove all non-rect attributes
    node.attributes_mut().retain(|a| {
           is_basic_shapes_attr(a)
        || a.id == AId::X
        || a.id == AId::Y
        || a.id == AId::Width
        || a.id == AId::Height
        || a.id == AId::Rx
        || a.id == AId::Ry
    });
}

fn process_circle(node: &Node) {
    // remove all non-circle attributes
    node.attributes_mut().retain(|a| {
           is_basic_shapes_attr(a)
        || a.id == AId::Cx
        || a.id == AId::Cy
        || a.id == AId::R
    });
}

fn process_ellipse(node: &Node) {
    // remove all non-ellipse attributes
    node.attributes_mut().retain(|a| {
           is_basic_shapes_attr(a)
        || a.id == AId::Cx
        || a.id == AId::Cy
        || a.id == AId::Rx
        || a.id == AId::Ry
    });
}

fn process_line(node: &Node) {
    // remove all non-line attributes
    node.attributes_mut().retain(|a| {
           is_basic_shapes_attr(a)
        || a.id == AId::X1
        || a.id == AId::Y1
        || a.id == AId::X2
        || a.id == AId::Y2
    });
}

fn process_poly(node: &Node) {
    // remove all non-polyline/polygon attributes
    node.attributes_mut().retain(|a| {
           is_basic_shapes_attr(a)
        || a.id == AId::Points
    });
}

fn is_basic_shapes_attr(a: &Attribute) -> bool {
    // list of common basic shapes attributes
    // https://www.w3.org/TR/SVG/shapes.html#RectElement

       a.is_conditional_processing()
    || a.is_core()
    || a.is_graphical_event()
    || a.is_presentation()
    || a.id == AId::Class
    || a.id == AId::Style
    || a.id == AId::ExternalResourcesRequired
    || a.id == AId::Transform
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, WriteToString};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_data($in_text).unwrap();
                remove_needless_attributes(&doc);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    macro_rules! test_eq {
        ($name:ident, $in_text:expr) => (
            test!($name, $in_text, String::from_utf8_lossy($in_text));
        )
    }

    test!(rm_clip_path_attrs_1,
b"<svg>
    <clipPath>
        <rect width='10' fill='red'/>
    </clipPath>
</svg>",
"<svg>
    <clipPath>
        <rect width='10'/>
    </clipPath>
</svg>
");

    test!(rm_clip_path_attrs_2,
b"<svg>
    <clipPath>
        <text fill='red' font-size='10'/>
    </clipPath>
</svg>",
"<svg>
    <clipPath>
        <text font-size='10'/>
    </clipPath>
</svg>
");

    test!(rm_rect_attrs_1,
b"<svg>
    <rect dx='10' fill='#ff0000' r='5'/>
</svg>",
"<svg>
    <rect fill='#ff0000'/>
</svg>
");

}

