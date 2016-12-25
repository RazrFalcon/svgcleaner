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

use svgdom::{Document, Node, AttributeValue};
use svgdom::types::Length;

/// We don't remove invalid elements, just make them invisible.
/// Then they can be removed via `remove_invisible_elements`.
pub fn fix_invalid_attributes(doc: &Document) {
    for node in doc.descendants().svg() {
        // descendants() iterates only over svg elements, which all have a tag name
        match node.tag_id().unwrap() {
            EId::Rect => fix_rect(&node),
            EId::Polyline | EId::Polygon => fix_poly(&node),
            EId::LinearGradient | EId::RadialGradient => fix_stops(&node),
            _ => {}
        }
    }
}

fn fix_rect(node: &Node) {
    // fix attributes according to: https://www.w3.org/TR/SVG/shapes.html#RectElement

    fix_len(node, AId::Width,  Length::zero());
    fix_len(node, AId::Height, Length::zero());

    rm_negative_len(node, AId::Rx);
    rm_negative_len(node, AId::Ry);

    // TODO: check that 'rx <= widht/2' and 'ry <= height/2'
    // Remember: a radius attributes can have different units,
    // so we need can't compare them. Probably we can do this only
    // after converting all units to px, which is optional.
}

fn fix_poly(node: &Node) {
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

fn fix_len(node: &Node, id: AId, new_len: Length) {
    if node.has_attribute(id) {
        fix_negative_len(node, id, new_len);
    } else {
        node.set_attribute(id, new_len);
    }
}

fn fix_negative_len(node: &Node, id: AId, new_len: Length) {
    if let Some(av) = node.attribute_value(id) {
        // unwrap is safe, because coordinates must have a Length type
        let l = av.as_length().unwrap();
        if l.num.is_sign_negative() {
            node.set_attribute(id, new_len);
        }
    }
}

fn rm_negative_len(node: &Node, id: AId) {
    if let Some(av) = node.attribute_value(id) {
        // unwrap is safe, because coordinates must have a Length type
        let l = av.as_length().unwrap();
        if l.num.is_sign_negative() {
            node.remove_attribute(id);
        }
    }
}

fn fix_stops(node: &Node) {
    // https://www.w3.org/TR/SVG/pservers.html#StopElementOffsetAttribute

    let mut prev_offset = 0.0;

    for child in node.children() {
        // unwrap is safe, because 'offset' already resolved in 'resolve_attrs'
        let mut offset = *child.attribute_value(AId::Offset).unwrap().as_number().unwrap();

        if offset < 0.0 {
            offset = 0.0;
        } else if offset > 1.0 {
            offset = 1.0;
        }

        if offset < prev_offset {
            offset = prev_offset;
        }

        child.set_attribute(AId::Offset, offset);

        prev_offset = offset;
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, WriteToString};
    use task::resolve_attributes;

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_data($in_text).unwrap();
                resolve_attributes(&doc).unwrap();
                fix_invalid_attributes(&doc);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    test!(fix_rect_1,
b"<svg>
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
b"<svg>
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

    test!(fix_polyline_1,
b"<svg>
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

    test!(fix_stop_1,
b"<svg>
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
