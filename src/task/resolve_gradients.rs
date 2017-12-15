// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

use svgdom::{
    Attribute,
    AttributeId,
    AttributeValue,
    Document,
    ElementId,
    ElementType,
    Node,
    ValueId,
};
use svgdom::types::{
    Color,
    Length,
    LengthUnit,
};

use error::{
    ErrorKind,
    Result,
};

/// Resolve attributes of `linearGradient` elements.
///
/// According to the SVG spec, `linearGradient` attributes can be
/// inherited via `xlink:href` attribute.
/// So we have to search linked gradients first and if they do not have such attributes
/// we have to fallback to the default one.
///
/// This method will process all `linearGradient` elements in the `Document`.
///
/// Resolvable attributes: `x1`, `y1`, `x2`, `y2`, `gradientUnits`,
/// `gradientTransform`, `spreadMethod`.
///
/// Details: https://www.w3.org/TR/SVG/pservers.html#LinearGradients
pub fn resolve_linear_gradient_attributes(doc: &Document) {
    for node in &mut gen_order(doc, ElementId::LinearGradient) {
        check_attr(node, AttributeId::GradientUnits,
            Some(AttributeValue::from(ValueId::ObjectBoundingBox)));
        check_attr(node, AttributeId::SpreadMethod, Some(AttributeValue::from(ValueId::Pad)));
        check_attr(node, AttributeId::X1, Some(AttributeValue::from((0.0, LengthUnit::Percent))));
        check_attr(node, AttributeId::Y1, Some(AttributeValue::from((0.0, LengthUnit::Percent))));
        check_attr(node, AttributeId::X2, Some(AttributeValue::from((100.0, LengthUnit::Percent))));
        check_attr(node, AttributeId::Y2, Some(AttributeValue::from((0.0, LengthUnit::Percent))));
        check_attr(node, AttributeId::GradientTransform, None);
    }
}

/// Resolve attributes of `radialGradient` elements.
///
/// According to the SVG spec, `radialGradient` attributes can be
/// inherited via `xlink:href` attribute.
/// So we have to search linked gradients first and if they do not have such attributes
/// we have to fallback to the default one.
///
/// This method will process all `radialGradient` elements in the `Document`.
///
/// Resolvable attributes: `cx`, `cy`, `fx`, `fy`, `r`, `gradientUnits`,
/// `gradientTransform`, `spreadMethod`.
///
/// Details: https://www.w3.org/TR/SVG/pservers.html#RadialGradients
pub fn resolve_radial_gradient_attributes(doc: &Document) {
    // We trying to find 'fx', 'fy' in referenced nodes first,
    // and if they not found - we get it from current 'cx', 'cy'.
    // But if we resolve referenced node first, it will have own
    // 'fx', 'fy' which we will inherit, instead of nodes own.
    // Which will lead to rendering error.
    // So we need to resolve nodes in referencing order.
    // From not referenced to referenced.

    for node in &mut gen_order(doc, ElementId::RadialGradient) {
        check_attr(node, AttributeId::GradientUnits,
            Some(AttributeValue::from(ValueId::ObjectBoundingBox)));
        check_attr(node, AttributeId::SpreadMethod, Some(AttributeValue::from(ValueId::Pad)));
        check_attr(node, AttributeId::Cx, Some(AttributeValue::from((50.0, LengthUnit::Percent))));
        check_attr(node, AttributeId::Cy, Some(AttributeValue::from((50.0, LengthUnit::Percent))));
        check_attr(node, AttributeId::R,  Some(AttributeValue::from((50.0, LengthUnit::Percent))));

        let cx = node.attributes().get_value(AttributeId::Cx).cloned();
        let cy = node.attributes().get_value(AttributeId::Cy).cloned();
        check_attr(node, AttributeId::Fx, cx);
        check_attr(node, AttributeId::Fy, cy);

        check_attr(node, AttributeId::GradientTransform, None);
    }
}

/// Resolve attributes of `stop` elements.
///
/// This method will process all `stop` elements in the `Document`.
///
/// Resolvable attributes: `offset`, `stop-color`, `stop-opacity`.
///
/// Details: https://www.w3.org/TR/SVG/pservers.html#GradientStops
///
/// # Errors
///
/// Will return `Error::MissingAttribute` if `stop` element,
/// which is not a first child of a gradient, didn't have an `offset` attribute.
pub fn resolve_stop_attributes(doc: &Document) -> Result<()> {
    for gradient in doc.descendants().filter(|n| n.is_gradient()) {
        for (idx, mut node) in gradient.children().enumerate() {
            let av = node.attributes().get_value(AttributeId::Offset).cloned();
            if let Some(AttributeValue::Length(l)) = av {
                if l.unit == LengthUnit::Percent {
                    // convert percent into a number
                    let new_l = Length::new_number(l.num / 100.0);
                    node.set_attribute((AttributeId::Offset, new_l));
                }
            } else {
                if idx == 0 {
                    // Allow first stop to not have an offset.
                    warn!("The 'stop' element must have an 'offset' attribute. \
                           Fallback to 'offset=0'.");
                    node.set_attribute((AttributeId::Offset, Length::zero()));
                } else {
                    return Err(ErrorKind::MissingAttribute("stop".to_string(),
                                                           "offset".to_string()).into());
                }
            }

            if !node.has_attribute(AttributeId::StopColor) {
                let mut a = Attribute::new(AttributeId::StopColor, Color::new(0, 0, 0));
                a.visible = false;
                node.set_attribute(a);
            }

            if !node.has_attribute(AttributeId::StopOpacity) {
                let mut a = Attribute::new(AttributeId::StopOpacity, 1.0);
                a.visible = false;
                node.set_attribute(a);
            }
        }
    }

    Ok(())
}

// TODO: explain algorithm
fn gen_order(doc: &Document, eid: ElementId) -> Vec<Node> {
    let nodes = doc.descendants().filter(|n| n.is_tag_name(eid))
                   .collect::<Vec<Node>>();

    let mut order = Vec::with_capacity(nodes.len());

    while order.len() != nodes.len() {
        for node in &nodes {
            if order.iter().any(|on| on == node) {
                continue;
            }

            let c = node.linked_nodes().filter(|n| {
                n.is_tag_name(eid) && !order.iter().any(|on| on == n)
            }).count();

            if c == 0 {
                order.push(node.clone());
            }
        }
    }

    order
}

fn check_attr(node: &mut Node, id: AttributeId, def_value: Option<AttributeValue>) {
    if !node.has_attribute(id) {
        if let Some(v) = resolve_attribute(node, id, def_value) {
            let mut a = Attribute::new(id, v);
            a.visible = false;
            node.set_attribute(a);
        }
    }
}

fn resolve_attribute(node: &Node, id: AttributeId, def_value: Option<AttributeValue>)
                     -> Option<AttributeValue> {
    if node.has_attribute(id) {
        return node.attributes().get_value(id).cloned();
    }

    match node.attributes().get_value(AttributeId::XlinkHref) {
        Some(av) => {
            match *av {
                AttributeValue::Link(ref ref_node) => resolve_attribute(ref_node, id, def_value),
                _ => unreachable!(),
            }
        }
        None => {
            match node.attributes().get_value(id) {
                Some(v) => Some(v.clone()),
                None => def_value,
            }
        }
    }
}

#[cfg(test)]
macro_rules! base_test {
    ($name:ident, $functor:expr, $in_text:expr, $out_text:expr) => (
        #[test]
        fn $name() {
            let doc = Document::from_str($in_text).unwrap();
            $functor(&doc);
            let mut opt = write_opt_for_tests!();
            opt.write_hidden_attributes = true;
            assert_eq_text!(doc.to_string_with_opt(&opt), $out_text);
        }
    )
}

#[cfg(test)]
mod lg_tests {
    use super::*;
    use svgdom::{Document, ToStringWithOptions};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, resolve_linear_gradient_attributes, $in_text, $out_text);
        )
    }

    test!(resolve_1,
"<svg>
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
    use svgdom::{Document, ToStringWithOptions};

    macro_rules! test_rg {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, resolve_radial_gradient_attributes, $in_text, $out_text);
        )
    }

    test_rg!(resolve_1,
"<svg>
    <radialGradient id='rg1'/>
</svg>",
"<svg>
    <radialGradient id='rg1' cx='50%' cy='50%' fx='50%' fy='50%' gradientUnits='objectBoundingBox' \
        r='50%' spreadMethod='pad'/>
</svg>
");

    test_rg!(resolve_2,
"<svg>
    <radialGradient id='rg1' cx='10' cy='20'/>
</svg>",
"<svg>
    <radialGradient id='rg1' cx='10' cy='20' fx='10' fy='20' gradientUnits='objectBoundingBox' \
        r='50%' spreadMethod='pad'/>
</svg>
");

    test_rg!(resolve_3,
"<svg>
    <radialGradient id='rg1' cx='10' cy='20' fx='30' fy='40' \
        gradientTransform='matrix(1 0 0 1 10 20)' gradientUnits='userSpaceOnUse' r='5' \
        spreadMethod='repeat'/>
    <radialGradient id='rg2' xlink:href='#rg1'/>
</svg>",
"<svg>
    <radialGradient id='rg1' cx='10' cy='20' fx='30' fy='40' \
        gradientTransform='translate(10 20)' gradientUnits='userSpaceOnUse' r='5' \
        spreadMethod='repeat'/>
    <radialGradient id='rg2' cx='10' cy='20' fx='30' fy='40' \
        gradientTransform='translate(10 20)' gradientUnits='userSpaceOnUse' r='5' \
        spreadMethod='repeat' xlink:href='#rg1'/>
</svg>
");

    test_rg!(resolve_4,
"<svg>
    <radialGradient id='rg1' cx='10' cy='20' fx='30' fy='40' \
        gradientTransform='matrix(1 0 0 1 10 20)' gradientUnits='userSpaceOnUse' r='5' \
        spreadMethod='repeat'/>
    <radialGradient id='rg2' xlink:href='#rg1'/>
    <radialGradient id='rg3' xlink:href='#rg2'/>
</svg>",
"<svg>
    <radialGradient id='rg1' cx='10' cy='20' fx='30' fy='40' \
        gradientTransform='translate(10 20)' gradientUnits='userSpaceOnUse' r='5' \
        spreadMethod='repeat'/>
    <radialGradient id='rg2' cx='10' cy='20' fx='30' fy='40' \
        gradientTransform='translate(10 20)' gradientUnits='userSpaceOnUse' r='5' \
        spreadMethod='repeat' xlink:href='#rg1'/>
    <radialGradient id='rg3' cx='10' cy='20' fx='30' fy='40' \
        gradientTransform='translate(10 20)' gradientUnits='userSpaceOnUse' r='5' \
        spreadMethod='repeat' xlink:href='#rg2'/>
</svg>
");

    test_rg!(resolve_5,
"<svg>
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
"<svg>
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
"<svg>
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
    use svgdom::{Document, ToStringWithOptions};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_str($in_text).unwrap();
                resolve_stop_attributes(&doc).unwrap();
                let mut opt = write_opt_for_tests!();
                opt.write_hidden_attributes = true;
                assert_eq_text!(doc.to_string_with_opt(&opt), $out_text);
            }
        )
    }

    test!(resolve_1,
"<svg>
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
