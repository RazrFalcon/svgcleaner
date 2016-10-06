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

use super::short::{AId};

use svgdom::{Document, Node, AttributeValue, ValueId};

pub fn remove_gradient_attributes(doc: &Document) {
    // TODO: process coordinates and transform
    //       we will get a small impact on a cleaning ratio, so keep it for the next release
    process_units(doc);
}

fn process_units(doc: &Document) {
    rm_equal(doc);
    group_to_parent(doc);
}

// if a gradient has the same 'gradientUnits' value as a parent
// we can remove such attribute
fn rm_equal(doc: &Document) {
    let mut order = Vec::new();
    for node in doc.descendants().filter(|n|    super::is_gradient(n)
                                             && n.has_attribute(AId::XlinkHref)) {
        let c = node.linked_nodes().filter(|n| super::is_gradient(n)).count();
        // the gradient element and count of gradient elements than uses it
        order.push((node.clone(), c));
    }

    let mut sub_order = Vec::new();

    // process gradients from less used to most used
    while !order.is_empty() {
        for &(ref node, count) in &order {
            if count == 0 {
                // collect gradients that doesn't used by any other gradients
                // usage depend on the 'count' value and not on the real usage
                sub_order.push(node.clone());

                if let Some(av) = node.attribute_value(AId::XlinkHref) {
                    if let AttributeValue::Link(link) = av {
                        // if current units is equal to parent units we can remove them
                        if node.attribute_value(AId::GradientUnits)
                                == link.attribute_value(AId::GradientUnits) {
                            make_attr_invisible(node, AId::GradientUnits);
                        }
                    }
                }
            }
        }

        // remove unused gradients
        order.retain(|&(_, c)| c > 0);

        // decrease usage count of processed gradients
        for n in &sub_order {
            if let Some(av) = n.attribute_value(AId::XlinkHref) {
                if let AttributeValue::Link(link) = av {
                    for &mut (ref node, ref mut count) in &mut order {
                        if *node == link {
                            *count -= 1;
                        }
                    }
                }
            }
        }

        sub_order.clear();
    }
}

// if several gradients linked to the same gradient
// we can move their 'gradientUnits' to the parent
fn group_to_parent(doc: &Document) {
    let nodes: Vec<Node> = doc.descendants()
                              .filter(|n| super::is_gradient(n))
                              .filter(|n| !n.has_attribute(AId::XlinkHref))
                              .filter(|n| n.linked_nodes().all(|l| super::is_gradient(&l)))
                              .collect();

    for node in &nodes {
        let total_count = node.linked_nodes().count();
        let count = node.linked_nodes()
                        .filter(|n| n.has_attribute_with_value(AId::GradientUnits,
                                                               ValueId::ObjectBoundingBox))
                        .count();

        if count == total_count {
            // if all linked gradients has the 'objectBoundingBox' value - move
            // it to the parent and remove from linked

            node.set_attribute(AId::GradientUnits, ValueId::ObjectBoundingBox);

            for n in node.linked_nodes() {
                make_attr_invisible(&n, AId::GradientUnits);
            }
        } else if count == 0 {
            // if all linked gradients has the 'userSpaceOnUse' value - move
            // it to the parent and remove from linked

            node.set_attribute(AId::GradientUnits, ValueId::UserSpaceOnUse);

            for n in node.linked_nodes() {
                make_attr_invisible(&n, AId::GradientUnits);
            }
        } else if count >= (total_count as f32 / 2.0).round() as usize {
            // if most linked gradients has the 'objectBoundingBox' value - move
            // it to the parent and remove from linked

            node.set_attribute(AId::GradientUnits, ValueId::ObjectBoundingBox);

            for n in node.linked_nodes() {
                if let Some(v) = n.attribute_value(AId::GradientUnits) {
                    if v == AttributeValue::PredefValue(ValueId::ObjectBoundingBox) {
                        make_attr_invisible(&n, AId::GradientUnits);
                    } else {
                        n.set_attribute(AId::GradientUnits, ValueId::UserSpaceOnUse);
                    }
                }
            }
        } else {
            // if most linked gradients has the 'userSpaceOnUse' value - move
            // it to the parent and remove from linked

            node.set_attribute(AId::GradientUnits, ValueId::UserSpaceOnUse);

            for n in node.linked_nodes() {
                if let Some(v) = n.attribute_value(AId::GradientUnits) {
                    if v == AttributeValue::PredefValue(ValueId::UserSpaceOnUse) {
                        make_attr_invisible(&n, AId::GradientUnits);
                    } else {
                        n.set_attribute(AId::GradientUnits, ValueId::ObjectBoundingBox);
                    }
                }
            }
        }

        if let Some(v) = node.attribute_value(AId::GradientUnits) {
            if v == AttributeValue::PredefValue(ValueId::ObjectBoundingBox) {
                make_attr_invisible(&node, AId::GradientUnits);
            }
        }
    }
}

fn make_attr_invisible(node: &Node, aid: AId) {
    let mut attrs = node.attributes_mut();
    attrs.get_mut(aid).unwrap().visible = false;
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
                remove_gradient_attributes(&doc);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    macro_rules! test_eq {
        ($name:ident, $in_text:expr) => (
            test!($name, $in_text, String::from_utf8_lossy($in_text));
        )
    }

    test!(move_gradient_units_1,
b"<svg>
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
b"<svg>
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
b"<svg>
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
b"<svg>
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
b"<svg>
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
b"<svg>
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
