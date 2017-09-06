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
use svgdom::types::Transform;

use task::short::AId;

pub use self::linear_gradient::remove_dupl_linear_gradients;
pub use self::radial_gradient::remove_dupl_radial_gradients;
pub use self::fe_gaussian_blur::remove_dupl_fe_gaussian_blur;

mod linear_gradient;
mod radial_gradient;
mod fe_gaussian_blur;

macro_rules! check_attr {
    ($attrs1:expr, $attrs2:expr, $id:expr, $def:expr) => ({
        let def = AttributeValue::from($def);
        if     $attrs1.get_value($id).unwrap_or(&def)
            != $attrs2.get_value($id).unwrap_or(&def) {
            return false;
        }
    })
}

fn rm_loop<F>(nodes: &mut Vec<Node>, cmp: F)
    where F : Fn(&Node, &Node) -> bool
{
    let mut link_attrs: Vec<(Node, AId, Node)> = Vec::new();

    let mut len = nodes.len();
    let mut i1 = 0;
    while i1 < len {
        let node1 = nodes[i1].clone();

        let mut i2 = i1 + 1;
        while i2 < len {
            let mut node2 = nodes[i2].clone();
            i2 += 1;

            if !cmp(&node1, &node2) {
                continue;
            }

            // Collect linked nodes.
            for ln in node2.linked_nodes() {
                let attrs = ln.attributes();

                for attr in attrs.iter() {
                    match attr.value {
                        AttributeValue::Link(ref n) | AttributeValue::FuncLink(ref n) => {
                            if *n == node2 {
                                link_attrs.push((ln.clone(), attr.id().unwrap(), node1.clone()));
                            }
                        }
                        _ => {}
                    }
                }
            }

            // Relink nodes.
            if !link_attrs.is_empty() {
                for &mut (ref mut ln, ref aid, ref n) in &mut link_attrs {
                    if *ln.id() != *n.id() {
                        ln.set_attribute((*aid, n.clone()));
                    }
                }
                link_attrs.clear();
            }

            node2.remove();

            nodes.remove(i2 - 1);
            i2 -= 1;
            len -= 1;
        }

        i1 += 1;
    }
}

fn is_gradient_attrs_equal(node1: &Node, node2: &Node, attrs: &[AId]) -> bool {
    let attrs1 = node1.attributes();
    let attrs2 = node2.attributes();

    check_attr!(&attrs1, &attrs2, AId::GradientTransform, Transform::default());

    if attrs1.contains(AId::XlinkHref) && attrs2.contains(AId::XlinkHref) {
        if attrs1.get_value(AId::XlinkHref).unwrap() != attrs2.get_value(AId::XlinkHref).unwrap() {
            return false;
        }
    }

    for aid in attrs.iter() {
        if attrs1.get_value(*aid).unwrap() != attrs2.get_value(*aid).unwrap() {
            return false;
        }
    }

    true
}

pub fn is_equal_stops(node1: &Node, node2: &Node) -> bool {
    let children_count1 = node1.children().count();

    if children_count1 != node2.children().count() {
        return false;
    }

    if children_count1 == 0 {
        return true;
    }

    let iter1 = node1.children();
    let iter2 = node2.children();

    for (mut c1, mut c2) in iter1.zip(iter2) {
        let attrs1 = c1.attributes_mut();
        let attrs2 = c2.attributes_mut();

        if !(   attrs1.get_value(AId::Offset) == attrs2.get_value(AId::Offset)
             && attrs1.get_value(AId::StopColor) == attrs2.get_value(AId::StopColor)
             && attrs1.get_value(AId::StopOpacity) == attrs2.get_value(AId::StopOpacity)) {
            return false;
        }
    }

    true
}

#[cfg(test)]
mod tests {
    use svgdom::{Document, Node};
    use task::short::EId;
    use task;

    macro_rules! test {
        ($name:ident, $in_text:expr, $flag:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_str($in_text).unwrap();
                task::resolve_linear_gradient_attributes(&doc);
                task::resolve_radial_gradient_attributes(&doc);
                task::resolve_stop_attributes(&doc).unwrap();
                let vec = doc.descendants().filter(|n| n.is_tag_name(EId::LinearGradient))
                             .collect::<Vec<Node>>();

                assert_eq!(super::is_equal_stops(&vec[0], &vec[1]), $flag);
            }
        )
    }

    test!(cmp_1,
"<svg>
    <linearGradient id='lg1'>
        <stop offset='0' stop-color='#ff0000' stop-opacity='1'/>
        <stop offset='1' stop-color='#000000' stop-opacity='1'/>
    </linearGradient>
    <linearGradient id='lg2'>
        <stop offset='0' stop-color='#ff0000' stop-opacity='1'/>
        <stop offset='1' stop-color='#000000' stop-opacity='1'/>
    </linearGradient>
</svg>", true);

    test!(cmp_2,
"<svg>
    <linearGradient id='lg1'>
        <stop offset='0' stop-color='#ff0000'/>
        <stop offset='1' stop-color='#000000'/>
    </linearGradient>
    <linearGradient id='lg2'>
        <stop offset='0' stop-color='#ff0000' stop-opacity='1'/>
        <stop offset='1'/>
    </linearGradient>
</svg>", true);

    test!(cmp_3,
"<svg>
    <linearGradient id='lg1'>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
    <linearGradient id='lg2'>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
</svg>", true);

    test!(cmp_4,
"<svg>
    <linearGradient id='lg1'>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
    <linearGradient id='lg2'>
        <stop offset='0'/>
        <stop offset='0.5'/>
    </linearGradient>
</svg>", false);

    test!(cmp_5,
"<svg>
    <linearGradient id='lg1'>
        <stop offset='0'/>
    </linearGradient>
    <linearGradient id='lg2'>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
</svg>", false);
}
