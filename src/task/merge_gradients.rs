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

use task::short::{EId, AId};
use task::utils;

static LG_ATTRIBUTES: &'static [AId] = &[
    AId::GradientUnits,
    AId::SpreadMethod,
    AId::GradientTransform,
    AId::X1,
    AId::Y1,
    AId::X2,
    AId::Y2,
];

static RG_ATTRIBUTES: &'static [AId] = &[
    AId::GradientUnits,
    AId::SpreadMethod,
    AId::GradientTransform,
    AId::Cx,
    AId::Cy,
    AId::Fx,
    AId::Fy,
    AId::R,
];

pub fn merge_gradients(doc: &Document) {
    let mut nodes = Vec::with_capacity(16);

    loop {
        _merge_gradients(doc, &mut nodes);

        if nodes.is_empty() {
            break;
        }

        utils::remove_nodes(&mut nodes);
    }
}

fn _merge_gradients(doc: &Document, nodes: &mut Vec<Node>) {
    let iter = doc.descendants().filter(|n| n.is_gradient());
    for mut node in iter {
        let linked_node;

        if let Some(av) = node.attributes().get_value(AId::XlinkHref) {
            if let AttributeValue::Link(ref link) = *av {
                if link.uses_count() == 1 && !link.has_attribute(AId::XlinkHref) {
                    linked_node = link.clone();
                } else {
                    continue;
                }
            } else {
                continue;
            }
        } else {
            // TODO: continue "tree" is ugly
            continue;
        }

        if !node.has_children() {
            // Append 'stop' elements only when we don't have any before.
            while let Some(mut child) = linked_node.first_child() {
                child.detach();
                node.append(&child);
            }
        }

        node.remove_attribute(AId::XlinkHref);

        {
            // Now we need to move attributes from removed gradient,
            // but since all attribute already been resolved by resolve_attributes(),
            // we only need to make them visible.

            // Do not process all attributes - only important.
            let aid_list = if node.is_tag_name(EId::LinearGradient) {
                &LG_ATTRIBUTES[..]
            } else {
                &RG_ATTRIBUTES[..]
            };

            let link_attrs = linked_node.attributes();
            let mut node_attrs = node.attributes_mut();

            for aid in aid_list {
                if let Some(attr) = link_attrs.get(*aid) {
                    // If an attribute of the removed gradient is invisible,
                    // then that means it was a default and we don't need it.
                    if !attr.visible {
                        continue;
                    }

                    if let Some(n_attr) = node_attrs.get_mut(attr.id().unwrap()) {
                        n_attr.visible = true;
                    }
                }
            }
        }

        nodes.push(linked_node.clone());
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, ToStringWithOptions};
    use task;

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_str($in_text).unwrap();
                task::resolve_linear_gradient_attributes(&doc);
                task::resolve_radial_gradient_attributes(&doc);
                task::resolve_stop_attributes(&doc).unwrap();
                merge_gradients(&doc);
                let mut opt = write_opt_for_tests!();
                opt.simplify_transform_matrices = true;
                assert_eq_text!(doc.to_string_with_opt(&opt), $out_text);
            }
        )
    }

    // There is nothing to merge - just remove it.
    test!(merge_1,
"<svg>
    <linearGradient id='lg1'/>
    <linearGradient xlink:href='#lg1'/>
</svg>",
"<svg>
    <linearGradient/>
</svg>
");

    // Move 'stop' elements.
    // Order is important.
    test!(merge_2,
"<svg>
    <linearGradient id='lg1'>
        <stop id='s1' offset='0'/>
        <stop id='s2' offset='1'/>
    </linearGradient>
    <linearGradient xlink:href='#lg1'/>
</svg>",
"<svg>
    <linearGradient>
        <stop id='s1' offset='0'/>
        <stop id='s2' offset='1'/>
    </linearGradient>
</svg>
");

    // Move attributes.
    test!(merge_3,
"<svg>
    <linearGradient id='lg1' x1='5' x2='5'>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
    <linearGradient x1='10' xlink:href='#lg1'/>
</svg>",
"<svg>
    <linearGradient x1='10' x2='5'>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
</svg>
");

    // Recursive.
    test!(merge_4,
"<svg>
    <linearGradient id='lg1' x1='5' x2='5'>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
    <linearGradient id='lg2' xlink:href='#lg1'/>
    <linearGradient x1='10' xlink:href='#lg2'/>
</svg>",
"<svg>
    <linearGradient x1='10' x2='5'>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
</svg>
");

    // Same as above, but in different order.
    test!(merge_5,
"<svg>
    <linearGradient x1='10' xlink:href='#lg2'/>
    <linearGradient id='lg2' xlink:href='#lg1'/>
    <linearGradient id='lg1' x1='5' x2='5'>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
</svg>",
"<svg>
    <linearGradient x1='10' x2='5'>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
</svg>
");

    // Move only element-specific attributes.
    test!(merge_6,
"<svg>
    <linearGradient id='lg1' x1='5' x2='5'/>
    <radialGradient xlink:href='#lg1'/>
</svg>",
"<svg>
    <radialGradient/>
</svg>
");

    // Skip existing stop's.
    test!(merge_7,
"<svg>
    <linearGradient id='lg1'>
        <stop id='s1' offset='0'/>
        <stop id='s2' offset='1'/>
    </linearGradient>
    <linearGradient id='lg2' xlink:href='#lg1'>
        <stop id='s3' offset='0'/>
        <stop id='s4' offset='1'/>
    </linearGradient>
</svg>",
"<svg>
    <linearGradient id='lg2'>
        <stop id='s3' offset='0'/>
        <stop id='s4' offset='1'/>
    </linearGradient>
</svg>
");

}
