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
    Node,
};
use svgdom::types::FuzzyEq;

use task::short::AId;

pub fn recalc_stroke(node: &mut Node, scale_factor: f64) {
    recalc_stroke_num(node, AId::StrokeWidth, scale_factor);
    recalc_stroke_dasharray(node, scale_factor);
    recalc_stroke_num(node, AId::StrokeDashoffset, scale_factor);
}

fn recalc_stroke_num(node: &mut Node, aid: AId, scale_factor: f64) {
    // Resolve current value.
    let value = if let Some(attr) = node.attributes().get(aid).cloned() {
        // Defined in the current node.
        attr.value
    } else {
        if let Some(n) = node.parents().find(|n| n.has_attribute(aid)) {
            // Defined in the parent node.
            n.attributes().get_value(aid).cloned().unwrap()
        } else {
            // Default value.
            AttributeValue::default_value(aid).unwrap()
        }
    };

    if let AttributeValue::Length(mut len) = value {
        if !len.num.is_fuzzy_zero() {
            len.num *= scale_factor;
            node.set_attribute((aid, len));
        }
    }
}

fn recalc_stroke_dasharray(node: &mut Node, scale_factor: f64) {
    let aid = AId::StrokeDasharray;

    // Resolve current 'stroke-dasharray'.
    let value = if let Some(attr) = node.attributes().get(aid).cloned() {
        // Defined in the current node.
        Some(attr.value)
    } else {
        if let Some(n) = node.parents().find(|n| n.has_attribute(aid)) {
            // Defined in the parent node.
            Some(n.attributes().get_value(aid).cloned().unwrap())
        } else {
            None
        }
    };

    if let Some(value) = value {
        if let AttributeValue::LengthList(mut list) = value {
            for n in &mut list {
                n.num *= scale_factor
            }
            node.set_attribute((aid, list));
        }
    }
}

pub fn remove_nodes(nodes: &mut Vec<Node>) {
    for n in nodes.iter_mut() {
        n.remove();
    }
    nodes.clear();
}

#[cfg(test)]
mod tests {
    use task::short::EId;
    use svgdom::{Document, ToStringWithOptions};
    use task::utils;

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_str($in_text).unwrap();
                let mut node = doc.descendants().find(|n| n.is_tag_name(EId::Path)).unwrap();
                utils::recalc_stroke(&mut node, 2.0);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    // From default.
    test!(recalc_stroke_1,
"<svg>
    <path/>
</svg>",
"<svg>
    <path stroke-width='2'/>
</svg>
");

    // From current.
    test!(recalc_stroke_2,
"<svg>
    <path stroke-width='2'/>
</svg>",
"<svg>
    <path stroke-width='4'/>
</svg>
");

    // From parent.
    test!(recalc_stroke_3,
"<svg stroke-width='2'>
    <path />
</svg>",
"<svg stroke-width='2'>
    <path stroke-width='4'/>
</svg>
");

    test!(recalc_stroke_4,
"<svg>
    <path stroke-dasharray='5 2 5 5 2 5'/>
</svg>",
"<svg>
    <path stroke-dasharray='10 4 10 10 4 10' stroke-width='2'/>
</svg>
");

    test!(recalc_stroke_5,
"<svg>
    <path stroke-dashoffset='2'/>
</svg>",
"<svg>
    <path stroke-dashoffset='4' stroke-width='2'/>
</svg>
");

    test!(recalc_stroke_6,
"<svg>
    <path stroke-dasharray='5 2 5 5 2 5' stroke-dashoffset='2'/>
</svg>",
"<svg>
    <path stroke-dasharray='10 4 10 10 4 10' stroke-dashoffset='4' stroke-width='2'/>
</svg>
");

}
