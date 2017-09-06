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
    ElementType,
    Node,
};
use svgdom::types::{
    Length,
};

use task::short::{AId, EId, Unit};

pub fn fix_invalid_attributes(doc: &Document) {
    for (id, mut node) in doc.descendants().svg() {
        match id {
            EId::Rect => fix_rect_attributes(&mut node),
            EId::Polyline | EId::Polygon => fix_poly_attributes(&mut node),
            EId::LinearGradient | EId::RadialGradient => fix_stop_attributes(&node),
            _ => {}
        }
    }
}

/// Fix `rect` element attributes.
///
/// - A negative `width` will be replaced with `0`
/// - A negative `height` will be replaced with `0`
/// - A negative `rx` will be removed
/// - A negative `ry` will be removed
///
/// Details: https://www.w3.org/TR/SVG/shapes.html#RectElement
pub fn fix_rect_attributes(node: &mut Node) {
    debug_assert!(node.is_tag_name(EId::Rect));

    fix_len(node, AId::Width,  Length::zero());
    fix_len(node, AId::Height, Length::zero());

    rm_negative_len(node, AId::Rx);
    rm_negative_len(node, AId::Ry);

    // TODO: check that 'rx <= widht/2' and 'ry <= height/2'
    // Remember: a radius attributes can have different units,
    // so we need can't compare them. Probably we can do this only
    // after converting all units to px, which is optional.
}

#[cfg(test)]
mod test_rect {
    use super::*;
    use super::EId;
    use svgdom::{Document, ToStringWithOptions};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_str($in_text).unwrap();
                for mut node in doc.descendants().filter(|n| n.is_tag_name(EId::Rect)) {
                    fix_rect_attributes(&mut node);
                }
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    test!(fix_rect_1,
"<svg>
    <rect/>
    <rect width='-1' height='-1'/>
    <rect width='30'/>
    <rect height='40'/>
    <rect width='-30'/>
    <rect height='-40'/>
    <rect width='0'/>
    <rect height='0'/>
</svg>",
"<svg>
    <rect height='0' width='0'/>
    <rect height='0' width='0'/>
    <rect height='0' width='30'/>
    <rect height='40' width='0'/>
    <rect height='0' width='0'/>
    <rect height='0' width='0'/>
    <rect height='0' width='0'/>
    <rect height='0' width='0'/>
</svg>
");

    test!(fix_rect_2,
"<svg>
    <rect height='50' width='40'/>
    <rect height='50' rx='-5' width='40'/>
    <rect height='50' ry='-5' width='40'/>
    <rect height='50' rx='-5' ry='-5' width='40'/>
</svg>",
"<svg>
    <rect height='50' width='40'/>
    <rect height='50' width='40'/>
    <rect height='50' width='40'/>
    <rect height='50' width='40'/>
</svg>
");

}

/// Fix `polyline` and `polygon` element attributes.
///
/// - An empty `points` attribute will be removed
/// - A `points` attribute with an odd number of coordinates will be truncated by one coordinate
///
/// Details: https://www.w3.org/TR/SVG/shapes.html#PolylineElement
/// https://www.w3.org/TR/SVG/shapes.html#PolygonElement
pub fn fix_poly_attributes(node: &mut Node) {
    debug_assert!(node.is_tag_name(EId::Polyline) || node.is_tag_name(EId::Polygon));

    let mut attrs_data = node.attributes_mut();
    let mut is_empty = false;

    if let Some(points_value) = attrs_data.get_value_mut(AId::Points) {
        if let AttributeValue::NumberList(ref mut p) = *points_value {
            if p.is_empty() {
                // remove if no points
                is_empty = true;
            } else if p.len() % 2 != 0 {
                // remove last point if points count is odd
                p.pop();

                // remove if no points
                if p.is_empty() {
                    is_empty = true;
                }
            }
        }
    }

    if is_empty {
        attrs_data.remove(AId::Points);
    }
}

#[cfg(test)]
mod test_poly {
    use super::*;
    use super::EId;
    use svgdom::{Document, ToStringWithOptions};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_str($in_text).unwrap();
                for (id, mut node) in doc.descendants().svg() {
                    if id == EId::Polygon || id == EId::Polyline {
                        fix_poly_attributes(&mut node);
                    }
                }
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    test!(fix_polyline_1,
"<svg>
    <polyline points='5 6 7'/>
    <polyline points='5'/>
    <polyline points=''/>
    <polyline/>
</svg>",
"<svg>
    <polyline points='5 6'/>
    <polyline/>
    <polyline/>
    <polyline/>
</svg>
");

}

/// Fix `stop` element attributes.
///
/// - A negative `offset` will be replaced with `0`
/// - An `offset` value bigger than `1` will be replaced with `1`
/// - An `offset` value smaller that previous will be set to previous
///
/// This method accepts `Node` with `linearGradient` or `radialGradient` tag name.
///
/// You should run this function only after
/// [`resolve_stop_attributes()`](fn.resolve_stop_attributes.html).
///
/// Details: https://www.w3.org/TR/SVG/pservers.html#StopElementOffsetAttribute
pub fn fix_stop_attributes(node: &Node) {
    debug_assert!(node.is_gradient());

    let mut prev_offset = 0.0;

    for mut child in node.children() {
        let av = child.attributes().get_value(AId::Offset).cloned();

        let mut offset = match av {
            Some(AttributeValue::Length(n)) => {
                if n.unit == Unit::None {
                    n.num
                } else {
                    unreachable!("'offset' must be resolved")
                }
            }
            _ => unreachable!("'offset' must be resolved"),
        };

        if offset < 0.0 {
            offset = 0.0;
        } else if offset > 1.0 {
            offset = 1.0;
        }

        if offset < prev_offset {
            offset = prev_offset;
        }

        child.set_attribute((AId::Offset, Length::new_number(offset)));

        prev_offset = offset;
    }
}

#[cfg(test)]
mod test_stop {
    use super::*;
    use svgdom::{Document, ToStringWithOptions, ElementType};
    use task::resolve_stop_attributes;

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_str($in_text).unwrap();
                resolve_stop_attributes(&doc).unwrap();
                for node in doc.descendants().filter(|n| n.is_gradient()) {
                    fix_stop_attributes(&node);
                }
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    test!(fix_stop_1,
"<svg>
    <linearGradient>
        <stop offset='-1'/>
        <stop offset='0.4'/>
        <stop offset='0.3'/>
        <stop offset='10'/>
        <stop offset='0.5'/>
    </linearGradient>
</svg>",
"<svg>
    <linearGradient>
        <stop offset='0'/>
        <stop offset='0.4'/>
        <stop offset='0.4'/>
        <stop offset='1'/>
        <stop offset='1'/>
    </linearGradient>
</svg>
");
}

fn fix_len(node: &mut Node, id: AId, new_len: Length) {
    if node.has_attribute(id) {
        fix_negative_len(node, id, new_len);
    } else {
        node.set_attribute((id, new_len));
    }
}

fn fix_negative_len(node: &mut Node, id: AId, new_len: Length) {
    let av = node.attributes().get_value(id).cloned();
    if let Some(AttributeValue::Length(l)) = av {
        if l.num.is_sign_negative() {
            node.set_attribute((id, new_len));
        }
    }
}

fn rm_negative_len(node: &mut Node, id: AId) {
    let av = node.attributes().get_value(id).cloned();
    if let Some(AttributeValue::Length(l)) = av {
        if l.num.is_sign_negative() {
            node.remove_attribute(id);
        }
    }
}
