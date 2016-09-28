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

use task::short::{EId, AId, Unit};
use error::CleanerError;

use svgdom::{Document, Node, Attribute, AttributeValue, ValueId};
use svgdom::types::Color;

pub fn linear_gradients(doc: &Document) {
    // https://www.w3.org/TR/SVG/pservers.html#LinearGradients

    for node in gen_order(doc, EId::LinearGradient) {
        check_attr(&node, AId::GradientUnits,
            Some(AttributeValue::from(ValueId::ObjectBoundingBox)));
        check_attr(&node, AId::SpreadMethod, Some(AttributeValue::from(ValueId::Pad)));
        check_attr(&node, AId::X1, Some(AttributeValue::from((0.0, Unit::Percent))));
        check_attr(&node, AId::Y1, Some(AttributeValue::from((0.0, Unit::Percent))));
        check_attr(&node, AId::X2, Some(AttributeValue::from((100.0, Unit::Percent))));
        check_attr(&node, AId::Y2, Some(AttributeValue::from((0.0, Unit::Percent))));
        check_attr(&node, AId::GradientTransform, None);
    }
}

pub fn radial_gradients(doc: &Document) {
    // https://www.w3.org/TR/SVG/pservers.html#RadialGradients

    // We trying to find 'fx', 'fy' in referenced nodes first,
    // and if they not found - we get it from current 'cx', 'cy'.
    // But if we resolve referenced node first, it will have own
    // 'fx', 'fy' which we will inherit, instead of nodes own.
    // Which will lead to rendering error.
    // So we need to resolve nodes in referencing order. From not referenced to referenced.

    for node in gen_order(doc, EId::RadialGradient) {
        check_attr(&node, AId::GradientUnits,
            Some(AttributeValue::from(ValueId::ObjectBoundingBox)));
        check_attr(&node, AId::SpreadMethod, Some(AttributeValue::from(ValueId::Pad)));
        check_attr(&node, AId::Cx, Some(AttributeValue::from((50.0, Unit::Percent))));
        check_attr(&node, AId::Cy, Some(AttributeValue::from((50.0, Unit::Percent))));
        check_attr(&node, AId::R,  Some(AttributeValue::from((50.0, Unit::Percent))));
        check_attr(&node, AId::Fx, node.attribute_value(AId::Cx));
        check_attr(&node, AId::Fy, node.attribute_value(AId::Cy));
        check_attr(&node, AId::GradientTransform, None);
    }
}

pub fn stop(doc: &Document) -> Result<(), CleanerError> {
    for (idx, node) in doc.descendants().filter(|n| n.is_tag_id(EId::Stop)).enumerate() {
        if !node.has_attribute(AId::Offset) {
            if idx == 0 {
                println!("Warning: The 'stop' element must have an 'offset' attribute. \
                          Fallback to 'offset=0'.");
                node.set_attribute(AId::Offset, 0);
            } else {
                return Err(CleanerError::MissingAttribute("stop".to_string(), "offset".to_string()));
            }
        } else {
            let a = node.attribute_value(AId::Offset).unwrap();
            let l = a.as_length().unwrap();
            if l.unit == Unit::Percent {
                // convert percent into number
                node.set_attribute(AId::Offset, l.num / 100.0);
            } else {
                // set original value too to change attribute type from Length to Number
                node.set_attribute(AId::Offset, l.num);
            }
        }

        if !node.has_attribute(AId::StopColor) {
            let mut a = Attribute::new(AId::StopColor, Color::new(0, 0, 0));
            a.visible = false;
            node.set_attribute_object(a);
        }

        if !node.has_attribute(AId::StopOpacity) {
            let mut a = Attribute::new(AId::StopOpacity, 1.0);
            a.visible = false;
            node.set_attribute_object(a);
        }
    }

    Ok(())
}

fn gen_order(doc: &Document, eid: EId) -> Vec<Node> {
    let nodes = doc.descendants().filter(|n| n.is_tag_id(eid))
                   .collect::<Vec<Node>>();

    let mut order = Vec::with_capacity(nodes.len());
    while order.len() != nodes.len() {
        for node in &nodes {
            if order.iter().any(|on| on == node) {
                continue;
            }

            let c = node.linked_nodes().filter(|n| {
                n.is_tag_id(eid) && !order.iter().any(|on| on == n)
            }).count();

            if c == 0 {
                order.push(node.clone());
            }
        }
    }

    order
}

fn check_attr(node: &Node, id: AId, def_value: Option<AttributeValue>) {
    if !node.has_attribute(id) {
        if let Some(v) = resolve_attribute(node, id, def_value) {
            let mut a = Attribute::new(id, v);
            a.visible = false;
            node.set_attribute_object(a);
        }
    }
}

fn resolve_attribute(node: &Node, id: AId, def_value: Option<AttributeValue>)
                     -> Option<AttributeValue> {
    if node.has_attribute(id) {
        return node.attribute_value(id);
    }

    match node.attribute_value(AId::XlinkHref) {
        Some(av) => {
            match av {
                AttributeValue::Link(ref_node) =>
                    return resolve_attribute(&ref_node, id, def_value),
                _ => unreachable!(),
            }
        }
        None => {
            match node.attribute_value(id) {
                Some(v) => return Some(v),
                None => return def_value,
            }
        }
    }

    def_value
}

#[cfg(test)]
macro_rules! base_test {
    ($name:ident, $functor:expr, $in_text:expr, $out_text:expr) => (
        #[test]
        #[allow(unused_must_use)]
        fn $name() {
            let doc = Document::from_data($in_text).unwrap();
            $functor(&doc);
            assert_eq_text!(doc_to_str_with_hidden!(doc), $out_text);
        }
    )
}

#[cfg(test)]
macro_rules! doc_to_str_with_hidden {
    ($doc:expr) => ({
        use svgdom::{WriteToString, WriteOptions};
        let mut opt = WriteOptions::default();
        opt.use_single_quote = true;
        opt.write_hidden_attributes = true;
        $doc.to_string_with_opt(&opt)
    })
}

#[cfg(test)]
mod lg_tests {
    use super::*;
    use svgdom::Document;

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, linear_gradients, $in_text, $out_text);
        )
    }

    test!(resolve_1,
b"<svg>
    <linearGradient id='lg1'/>
</svg>",
"<svg>
    <linearGradient id='lg1' gradientUnits='objectBoundingBox' spreadMethod='pad' \
        x1='0%' x2='100%' y1='0%' y2='0%'/>
</svg>
");

}

#[cfg(test)]
mod rg_tests {
    use super::*;
    use svgdom::{Document};

    macro_rules! test_rg {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, radial_gradients, $in_text, $out_text);
        )
    }

    test_rg!(resolve_1,
b"<svg>
    <radialGradient id='rg1'/>
</svg>",
"<svg>
    <radialGradient id='rg1' cx='50%' cy='50%' fx='50%' fy='50%' gradientUnits='objectBoundingBox' \
        r='50%' spreadMethod='pad'/>
</svg>
");

    test_rg!(resolve_2,
b"<svg>
    <radialGradient id='rg1' cx='10' cy='20'/>
</svg>",
"<svg>
    <radialGradient id='rg1' cx='10' cy='20' fx='10' fy='20' gradientUnits='objectBoundingBox' \
        r='50%' spreadMethod='pad'/>
</svg>
");

    test_rg!(resolve_3,
b"<svg>
    <radialGradient id='rg1' cx='10' cy='20' fx='30' fy='40' \
        gradientTransform='matrix(1 0 0 1 10 20)' gradientUnits='userSpaceOnUse' r='5' \
        spreadMethod='repeat'/>
    <radialGradient id='rg2' xlink:href='#rg1'/>
</svg>",
"<svg>
    <radialGradient id='rg1' cx='10' cy='20' fx='30' fy='40' \
        gradientTransform='matrix(1 0 0 1 10 20)' gradientUnits='userSpaceOnUse' r='5' \
        spreadMethod='repeat'/>
    <radialGradient id='rg2' cx='10' cy='20' fx='30' fy='40' \
        gradientTransform='matrix(1 0 0 1 10 20)' gradientUnits='userSpaceOnUse' r='5' \
        spreadMethod='repeat' xlink:href='#rg1'/>
</svg>
");

    test_rg!(resolve_4,
b"<svg>
    <radialGradient id='rg1' cx='10' cy='20' fx='30' fy='40' \
        gradientTransform='matrix(1 0 0 1 10 20)' gradientUnits='userSpaceOnUse' r='5' \
        spreadMethod='repeat'/>
    <radialGradient id='rg2' xlink:href='#rg1'/>
    <radialGradient id='rg3' xlink:href='#rg2'/>
</svg>",
"<svg>
    <radialGradient id='rg1' cx='10' cy='20' fx='30' fy='40' \
        gradientTransform='matrix(1 0 0 1 10 20)' gradientUnits='userSpaceOnUse' r='5' \
        spreadMethod='repeat'/>
    <radialGradient id='rg2' cx='10' cy='20' fx='30' fy='40' \
        gradientTransform='matrix(1 0 0 1 10 20)' gradientUnits='userSpaceOnUse' r='5' \
        spreadMethod='repeat' xlink:href='#rg1'/>
    <radialGradient id='rg3' cx='10' cy='20' fx='30' fy='40' \
        gradientTransform='matrix(1 0 0 1 10 20)' gradientUnits='userSpaceOnUse' r='5' \
        spreadMethod='repeat' xlink:href='#rg2'/>
</svg>
");

    test_rg!(resolve_5,
b"<svg>
    <radialGradient id='rg1' cx='10' cy='20' r='5'/>
    <radialGradient id='rg2' cy='30' xlink:href='#rg1'/>
</svg>",
"<svg>
    <radialGradient id='rg1' cx='10' cy='20' fx='10' fy='20' \
        gradientUnits='objectBoundingBox' r='5' spreadMethod='pad'/>
    <radialGradient id='rg2' cx='10' cy='30' fx='10' fy='30' \
        gradientUnits='objectBoundingBox' r='5' spreadMethod='pad' xlink:href='#rg1'/>
</svg>
");

    // This is the main test, because it check resolving order correctness.
    test_rg!(resolve_6,
b"<svg>
    <radialGradient id='rg2' cy='30' xlink:href='#rg1'/>
    <radialGradient id='rg3' cx='30' xlink:href='#rg2'/>
    <radialGradient id='rg4' cx='40' xlink:href='#rg2'/>
    <radialGradient id='rg1' cx='10' cy='20' r='5'/>
</svg>",
"<svg>
    <radialGradient id='rg2' cx='10' cy='30' fx='10' fy='30' \
        gradientUnits='objectBoundingBox' r='5' spreadMethod='pad' xlink:href='#rg1'/>
    <radialGradient id='rg3' cx='30' cy='30' fx='30' fy='30' \
        gradientUnits='objectBoundingBox' r='5' spreadMethod='pad' xlink:href='#rg2'/>
    <radialGradient id='rg4' cx='40' cy='30' fx='40' fy='30' \
        gradientUnits='objectBoundingBox' r='5' spreadMethod='pad' xlink:href='#rg2'/>
    <radialGradient id='rg1' cx='10' cy='20' fx='10' fy='20' \
        gradientUnits='objectBoundingBox' r='5' spreadMethod='pad'/>
</svg>
");

    test_rg!(resolve_7,
b"<svg>
    <linearGradient id='lg1' gradientUnits='userSpaceOnUse' spreadMethod='repeat' x='5' y='5'/>
    <radialGradient id='rg2' cy='30' r='5' xlink:href='#lg1'/>
</svg>",
"<svg>
    <linearGradient id='lg1' gradientUnits='userSpaceOnUse' spreadMethod='repeat' x='5' y='5'/>
    <radialGradient id='rg2' cx='50%' cy='30' fx='50%' fy='30' \
        gradientUnits='userSpaceOnUse' r='5' spreadMethod='repeat' xlink:href='#lg1'/>
</svg>
");
}

#[cfg(test)]
mod stop_tests {
    use super::*;
    use svgdom::Document;

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, stop, $in_text, $out_text);
        )
    }

    test!(resolve_1,
b"<svg>
    <linearGradient>
        <stop offset='50%'/>
    </linearGradient>
</svg>",
"<svg>
    <linearGradient>
        <stop offset='0.5' stop-color='#000000' stop-opacity='1'/>
    </linearGradient>
</svg>
");

}
