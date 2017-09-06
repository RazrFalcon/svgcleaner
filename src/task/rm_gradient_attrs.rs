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
    ValueId,
};

use task::short::AId;

pub fn remove_gradient_attributes(doc: &Document) {
    // TODO: process coordinates and transform
    //       we will get a small impact on a cleaning ratio, so keep it for a next release
    process_units(doc);
}

fn process_units(doc: &Document) {
    rm_equal(doc);
    group_to_parent(doc);
}

// If a gradient has the same 'gradientUnits' value as a parent
// we can remove such attribute.
fn rm_equal(doc: &Document) {
    let mut order = Vec::new();
    for node in doc.descendants().filter(|n|    n.is_gradient()
                                             && n.has_attribute(AId::XlinkHref)) {
        let c = node.linked_nodes().filter(|n| n.is_gradient()).count();
        // The gradient element and count of gradient elements than uses it.
        order.push((node.clone(), c));
    }

    let mut sub_order = Vec::new();

    // Process gradients from less used to most used.
    while !order.is_empty() {
        for &mut (ref mut node, count) in &mut order {
            if count == 0 {
                // Collect gradients that doesn't used by any other gradients
                // usage depend on the 'count' value and not on the real usage.
                sub_order.push(node.clone());

                let av = node.attributes().get_value(AId::XlinkHref).cloned();
                if let Some(av) = av {
                    if let AttributeValue::Link(ref link) = av {
                        // If current units is equal to parent units we can remove them.
                        if node.attributes().get_value(AId::GradientUnits)
                                == link.attributes().get_value(AId::GradientUnits) {
                            make_attr_invisible(node, AId::GradientUnits);
                        }
                    }
                }
            }
        }

        // Remove unused gradients.
        order.retain(|&(_, c)| c > 0);

        // Decrease usage count of processed gradients.
        for n in &sub_order {
            if let Some(av) = n.attributes().get_value(AId::XlinkHref) {
                if let AttributeValue::Link(ref link) = *av {
                    for &mut (ref node, ref mut count) in &mut order {
                        if node == link {
                            *count -= 1;
                        }
                    }
                }
            }
        }

        sub_order.clear();
    }
}

// If several gradients linked to the same gradient
// we can move their 'gradientUnits' to the parent.
fn group_to_parent(doc: &Document) {
    let mut nodes: Vec<Node> = doc.descendants()
                                  .filter(|n| n.is_gradient())
                                  .filter(|n| !n.has_attribute(AId::XlinkHref))
                                  .filter(|n| n.linked_nodes().all(|l| l.is_gradient()))
                                  .collect();

    for node in &mut nodes {
        let total_count = node.uses_count();
        let count = node.linked_nodes()
                        .filter(|n| {
                            n.attributes().get_value(AId::GradientUnits) ==
                                Some(&AttributeValue::PredefValue(ValueId::ObjectBoundingBox))
                        })
                        .count();

        if count == total_count {
            // If all linked gradients has the 'objectBoundingBox' value - move
            // it to the parent and remove from linked.

            node.set_attribute((AId::GradientUnits, ValueId::ObjectBoundingBox));

            for n in node.linked_nodes() {
                make_attr_invisible(&mut n.clone(), AId::GradientUnits);
            }
        } else if count == 0 {
            // If all linked gradients has the 'userSpaceOnUse' value - move
            // it to the parent and remove from linked.

            node.set_attribute((AId::GradientUnits, ValueId::UserSpaceOnUse));

            for n in node.linked_nodes() {
                make_attr_invisible(&mut n.clone(), AId::GradientUnits);
            }
        } else if count >= (total_count as f32 / 2.0).round() as usize {
            // If most linked gradients has the 'objectBoundingBox' value - move
            // it to the parent and remove from linked.

            node.set_attribute((AId::GradientUnits, ValueId::ObjectBoundingBox));

            for n in node.linked_nodes() {
                let av = n.attributes().get_value(AId::GradientUnits).cloned();
                if let Some(av) = av {
                    if av == AttributeValue::PredefValue(ValueId::ObjectBoundingBox) {
                        make_attr_invisible(&mut n.clone(), AId::GradientUnits);
                    } else {
                        n.clone().set_attribute((AId::GradientUnits, ValueId::UserSpaceOnUse));
                    }
                }
            }
        } else {
            // If most linked gradients has the 'userSpaceOnUse' value - move
            // it to the parent and remove from linked.

            node.set_attribute((AId::GradientUnits, ValueId::UserSpaceOnUse));

            for n in node.linked_nodes() {
                let av = n.attributes().get_value(AId::GradientUnits).cloned();
                if let Some(av) = av {
                    if av == AttributeValue::PredefValue(ValueId::UserSpaceOnUse) {
                        make_attr_invisible(&mut n.clone(), AId::GradientUnits);
                    } else {
                        n.clone().set_attribute((AId::GradientUnits, ValueId::ObjectBoundingBox));
                    }
                }
            }
        }

        let av = node.attributes().get_value(AId::GradientUnits).cloned();
        if let Some(av) = av {
            if av == AttributeValue::PredefValue(ValueId::ObjectBoundingBox) {
                make_attr_invisible(node, AId::GradientUnits);
            }
        }
    }
}

fn make_attr_invisible(node: &mut Node, aid: AId) {
    let mut attrs = node.attributes_mut();
    attrs.get_mut(aid).unwrap().visible = false;
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
                remove_gradient_attributes(&doc);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    test!(move_gradient_units_1,
"<svg>
    <linearGradient id='lg1'/>
    <linearGradient xlink:href='#lg1' gradientUnits='userSpaceOnUse'/>
    <radialGradient xlink:href='#lg1' gradientUnits='userSpaceOnUse'/>
</svg>",
"<svg>
    <linearGradient id='lg1' gradientUnits='userSpaceOnUse'/>
    <linearGradient xlink:href='#lg1'/>
    <radialGradient xlink:href='#lg1'/>
</svg>
");

    test!(move_1,
"<svg>
    <linearGradient id='lg1-1'/>
    <linearGradient id='lg2-1' gradientUnits='userSpaceOnUse' xlink:href='#lg1-1'/>
    <linearGradient id='lg2-2' gradientUnits='userSpaceOnUse' xlink:href='#lg1-1'/>
    <radialGradient id='lg3-1' gradientUnits='userSpaceOnUse' xlink:href='#lg2-1'/>
    <radialGradient id='lg3-2' gradientUnits='userSpaceOnUse' xlink:href='#lg2-1'/>
</svg>",
"<svg>
    <linearGradient id='lg1-1' gradientUnits='userSpaceOnUse'/>
    <linearGradient id='lg2-1' xlink:href='#lg1-1'/>
    <linearGradient id='lg2-2' xlink:href='#lg1-1'/>
    <radialGradient id='lg3-1' xlink:href='#lg2-1'/>
    <radialGradient id='lg3-2' xlink:href='#lg2-1'/>
</svg>
");

    test!(move_gradient_units_2,
"<svg>
    <linearGradient id='lg1'/>
    <linearGradient gradientUnits='userSpaceOnUse' xlink:href='#lg1'/>
    <linearGradient gradientUnits='userSpaceOnUse' xlink:href='#lg1'/>
    <linearGradient gradientUnits='objectBoundingBox' xlink:href='#lg1'/>
</svg>",
"<svg>
    <linearGradient id='lg1' gradientUnits='userSpaceOnUse'/>
    <linearGradient xlink:href='#lg1'/>
    <linearGradient xlink:href='#lg1'/>
    <linearGradient gradientUnits='objectBoundingBox' xlink:href='#lg1'/>
</svg>
");

    test!(move_gradient_units_3,
"<svg>
    <linearGradient id='lg1' gradientUnits='objectBoundingBox'/>
    <linearGradient gradientUnits='userSpaceOnUse' xlink:href='#lg1'/>
    <linearGradient gradientUnits='objectBoundingBox' xlink:href='#lg1'/>
    <linearGradient gradientUnits='objectBoundingBox' xlink:href='#lg1'/>
</svg>",
"<svg>
    <linearGradient id='lg1'/>
    <linearGradient gradientUnits='userSpaceOnUse' xlink:href='#lg1'/>
    <linearGradient xlink:href='#lg1'/>
    <linearGradient xlink:href='#lg1'/>
</svg>
");

    test!(move_gradient_units_4,
"<svg>
    <linearGradient id='lg1' gradientUnits='userSpaceOnUse'/>
    <linearGradient gradientUnits='userSpaceOnUse' xlink:href='#lg1'/>
    <linearGradient gradientUnits='objectBoundingBox' xlink:href='#lg1'/>
</svg>",
"<svg>
    <linearGradient id='lg1'/>
    <linearGradient gradientUnits='userSpaceOnUse' xlink:href='#lg1'/>
    <linearGradient xlink:href='#lg1'/>
</svg>
");

    test!(move_gradient_units_5,
"<svg>
    <linearGradient id='lg1'/>
    <linearGradient gradientUnits='userSpaceOnUse' xlink:href='#lg1'/>
    <linearGradient gradientUnits='userSpaceOnUse' xlink:href='#lg1'/>
    <linearGradient xlink:href='#lg1'/>
</svg>",
"<svg>
    <linearGradient id='lg1' gradientUnits='userSpaceOnUse'/>
    <linearGradient xlink:href='#lg1'/>
    <linearGradient xlink:href='#lg1'/>
    <linearGradient gradientUnits='objectBoundingBox' xlink:href='#lg1'/>
</svg>
");

}
