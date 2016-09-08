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

pub use self::linear_gradient::remove_dupl_linear_gradients;
pub use self::radial_gradient::remove_dupl_radial_gradients;

mod linear_gradient;
mod radial_gradient;

use task::short::{EId, AId, Unit};
use svgdom::types::{Transform, Color};
use svgdom::{Document, Node, AttributeValue};

macro_rules! check_attr {
    ($attrs1:expr, $attrs2:expr, $id:expr, $def:expr) => ({
        let def = AttributeValue::from($def);
        if     $attrs1.get_value($id.clone()).unwrap_or(&def)
            != $attrs2.get_value($id.clone()).unwrap_or(&def) {
            return false;
        }
    })
}

// macro_rules! borrow_attrs {
//     ($node:expr, $attrs:ident, $obj:ident) => (
//         let $obj = $node.attributes();
//         let $attrs = $obj.borrow();
//     )
// }

fn rm_loop(doc: &Document, eid: EId, attrs: &[AId]) {
    let mut lg_nodes = doc.descendants()
                        .filter(|ref n| n.is_tag_id(eid))
                        .collect::<Vec<Node>>();

    let mut len = lg_nodes.len();
    let mut i1 = 0;
    while i1 < len {
        let node1 = lg_nodes[i1].clone();

        let mut i2 = i1 + 1;
        while i2 < len {
            let node2 = lg_nodes[i2].clone();
            i2 += 1;

            if !is_attrs_equal(&node1, &node2, attrs) {
                continue;
            }

            if !is_equal_stops(&node1, &node2) {
                continue;
            }

            for ln in node2.linked_nodes() {
                match ln.find_reference_attribute(&node2) {
                    Some(aid) => {
                        ln.set_link_attribute(aid, node1.clone()).unwrap();
                    }
                    None => {}
                }
            }
            node2.detach();

            lg_nodes.remove(i2 - 1);
            i2 -= 1;
            len -= 1;
        }

        i1 += 1;
    }
}

fn is_attrs_equal(node1: &Node, node2: &Node, attrs: &[AId]) -> bool {
    // borrow_attrs!(node1, attrs1, attrs_obj1);
    // borrow_attrs!(node2, attrs2, attrs_obj2);

    let attrs1 = node1.attributes();
    let attrs2 = node2.attributes();

    check_attr!(&attrs1, &attrs2, AId::GradientTransform, Transform::default());

    if attrs1.contains(AId::XlinkHref) && attrs2.contains(AId::XlinkHref) {
        if attrs1.get_value(AId::XlinkHref).unwrap() != attrs2.get_value(AId::XlinkHref).unwrap() {
            return false;
        }
    }

    if attrs2.contains(AId::XlinkHref) {
        if attrs2.get_value(AId::XlinkHref).unwrap() != &AttributeValue::Link(node1.clone()) {
            return false;
        }
    }

    for aid in attrs.iter() {
        if attrs1.get_value(aid.clone()).unwrap() != attrs2.get_value(aid.clone()).unwrap() {
            return false;
        }
    }

    true
}

fn is_equal_stops(node1: &Node, node2: &Node) -> bool {
    if node1.children().count() != node2.children().count() {
        return false;
    }

    if node1.children().count() == 0 {
        return true;
    }

    let iter1 = node1.children();
    let iter2 = node2.children();

    for (c1, c2) in iter1.zip(iter2) {
        // borrow_attrs!(c1, attrs1, attrs_obj1);
        // borrow_attrs!(c2, attrs2, attrs_obj2);
        let attrs1 = c1.attributes_mut();
        let attrs2 = c2.attributes_mut();

        check_attr!(&attrs1, &attrs2, AId::Offset, (0.0, Unit::None));
        check_attr!(&attrs1, &attrs2, AId::StopColor, Color::new(0,0,0));
        check_attr!(&attrs1, &attrs2, AId::StopOpacity, 1.0);
    }

    true
}

#[cfg(test)]
mod tests {
    use svgdom::{Document, Node};
    use task::short::EId;

    macro_rules! test {
        ($name:ident, $in_text:expr, $flag:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_data($in_text).unwrap();
                let vec = doc.descendants().filter(|n| n.is_tag_id(EId::LinearGradient))
                             .collect::<Vec<Node>>();

                assert_eq!(super::is_equal_stops(&vec[0], &vec[1]), $flag);
            }
        )
    }

    test!(cmp_1,
b"<svg>
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
b"<svg>
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
b"<svg>
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
b"<svg>
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
b"<svg>
    <linearGradient id='lg1'>
        <stop offset='0'/>
    </linearGradient>
    <linearGradient id='lg2'>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
</svg>", false);
}
