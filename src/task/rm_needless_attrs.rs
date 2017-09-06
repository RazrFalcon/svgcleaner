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
    Attribute,
    AttributeType,
    AttributeValue,
    Document,
    ElementType,
    Node,
    ValueId,
};

use task::short::{EId, AId};

// TODO: split to suboptions

pub fn remove_needless_attributes(doc: &Document) {
    for (id, mut node) in doc.descendants().svg() {
        match id {
            EId::ClipPath => process_clip_path(&node),
            EId::Rect => process_rect(&mut node),
            EId::Circle => process_circle(&mut node),
            EId::Ellipse => process_ellipse(&mut node),
            EId::Line => process_line(&mut node),
            EId::Polyline | EId::Polygon => process_poly(&mut node),
            _ => {}
        }

        // We can remove any 'color' attributes since they
        // already resolved in 'resolve_inherit', which makes them pointless.
        node.remove_attribute(AId::Color);

        // 'enable-background' is only applicable to container elements.
        // https://www.w3.org/TR/SVG/filters.html#EnableBackgroundProperty
        if !node.is_container() && node.has_attribute(AId::EnableBackground) {
            node.remove_attribute(AId::EnableBackground);
        }

        process_fill(&mut node);
        process_stroke(&mut node);
        process_overflow(&mut node);
    }
}

fn process_clip_path(node: &Node) {
    for mut child in node.children() {
        if child.is_used() {
            continue;
        }

        // We can remove any fill and stroke based attributes
        // since they does not impact rendering.
        // https://www.w3.org/TR/SVG/masking.html#EstablishingANewClippingPath

        let mut ids = Vec::new();
        for (id, attr) in child.attributes().iter_svg() {
            if attr.is_fill() || attr.is_stroke() || attr.has_id(AId::Opacity) {
                ids.push(id);
            }
        }

        for id in &ids {
            child.remove_attribute(*id);
        }
    }
}

fn process_rect(node: &mut Node) {
    // Remove all non-rect attributes.
    node.attributes_mut().retain(|a| {
           is_basic_shapes_attr(a)
        || a.has_id(AId::X)
        || a.has_id(AId::Y)
        || a.has_id(AId::Width)
        || a.has_id(AId::Height)
        || a.has_id(AId::Rx)
        || a.has_id(AId::Ry)
    });
}

fn process_circle(node: &mut Node) {
    // Remove all non-circle attributes.
    node.attributes_mut().retain(|a| {
           is_basic_shapes_attr(a)
        || a.has_id(AId::Cx)
        || a.has_id(AId::Cy)
        || a.has_id(AId::R)
    });
}

fn process_ellipse(node: &mut Node) {
    // Remove all non-ellipse attributes.
    node.attributes_mut().retain(|a| {
           is_basic_shapes_attr(a)
        || a.has_id(AId::Cx)
        || a.has_id(AId::Cy)
        || a.has_id(AId::Rx)
        || a.has_id(AId::Ry)
    });
}

fn process_line(node: &mut Node) {
    // Remove all non-line attributes.
    node.attributes_mut().retain(|a| {
           is_basic_shapes_attr(a)
        || a.has_id(AId::X1)
        || a.has_id(AId::Y1)
        || a.has_id(AId::X2)
        || a.has_id(AId::Y2)
    });
}

fn process_poly(node: &mut Node) {
    // Remove all non-polyline/polygon attributes.
    node.attributes_mut().retain(|a| {
           is_basic_shapes_attr(a)
        || a.has_id(AId::Points)
    });
}

fn is_basic_shapes_attr(a: &Attribute) -> bool {
    // List of common basic shapes attributes.
    // https://www.w3.org/TR/SVG/shapes.html#RectElement

       a.is_conditional_processing()
    || a.is_core()
    || a.is_graphical_event()
    || a.is_presentation()
    || a.has_id(AId::Class)
    || a.has_id(AId::Style)
    || a.has_id(AId::ExternalResourcesRequired)
    || a.has_id(AId::Transform)
}

fn process_fill(node: &mut Node) {
    if !node.has_children() {
        // If 'fill' is disabled we can remove fill-based attributes.
        let av = node.attributes().get_value(AId::Fill).cloned();
        if let Some(av) = av {
            if av == AttributeValue::PredefValue(ValueId::None) {
                node.remove_attribute(AId::FillRule);
                node.remove_attribute(AId::FillOpacity);
            }
        }

        // If 'fill' is invisible we can disable fill completely.
        let av = node.attributes().get_value(AId::FillOpacity).cloned();
        if let Some(av) = av {
            if av == AttributeValue::Number(0.0) {
                node.set_attribute((AId::Fill, ValueId::None));
                node.remove_attribute(AId::FillRule);
                node.remove_attribute(AId::FillOpacity);
            }
        }
    }
}

static STROKE_ATTRIBUTES: &'static [AId] = &[
    AId::StrokeWidth,
    AId::StrokeLinecap,
    AId::StrokeLinejoin,
    AId::StrokeMiterlimit,
    AId::StrokeDasharray,
    AId::StrokeDashoffset,
    AId::StrokeOpacity,
];

fn process_stroke(node: &mut Node) {
    fn is_invisible(node: &Node) -> bool {
        // Skip nodes with marker, because it doesn't count opacity and stroke-width.
        if node.has_attributes(&[AId::Marker, AId::MarkerStart, AId::MarkerMid, AId::MarkerEnd]) {
            return false;
        }

        // The stroke will not be drawn if stoke-width is 0.
        if let Some(v) = node.attributes().get_value(AId::StrokeWidth) {
            if let AttributeValue::Length(l) = *v {
                if l.num == 0.0 {
                    return true;
                }
            }
        }

        // The stroke will not be drawn if stoke-opacity is 0.
        if let Some(v) = node.attributes().get_value(AId::StrokeOpacity) {
            if let AttributeValue::Number(num) = *v {
                if num == 0.0 {
                    return true;
                }
            }
        }

        false
    }

    if !node.has_children() {
        if is_invisible(node) {
            // Remove all stroke-based attributes if stroke is invisible.
            node.remove_attribute(AId::Stroke);
            node.remove_attributes(STROKE_ATTRIBUTES);

            // If the parent element defines stroke - we must mark current element
            // with 'none' stroke.
            if let Some(n) = node.parents().find(|n| n.has_attribute(AId::Stroke)) {
                let value = n.attributes().get_value(AId::Stroke).cloned().unwrap();
                if value != AttributeValue::PredefValue(ValueId::None) {
                    node.set_attribute((AId::Stroke, ValueId::None));
                }
            }
        } else {
            let av = node.attributes().get_value(AId::Stroke).cloned();
            if let Some(av) = av {
                if av == AttributeValue::PredefValue(ValueId::None) {
                    // Remove all stroke-based attributes, except 'stroke' itself,
                    // if the stroke is 'none'.
                    node.remove_attributes(STROKE_ATTRIBUTES);

                    // If the parent element doesn't define 'stroke' - we can remove it
                    // from the current element.
                    if node.parents().find(|n| n.has_attribute(AId::Stroke)).is_none() {
                        node.remove_attribute(AId::Stroke);
                    }
                }
            }
        }
    }
}

fn process_overflow(node: &mut Node) {
    // The 'overflow' property applies to elements that establish new viewports,
    // 'pattern' elements and 'marker' elements. For all other elements,
    // the property has no effect.
    //
    // https://www.w3.org/TR/SVG/masking.html#OverflowProperty
    match node.tag_id().unwrap() {
          EId::Svg
        | EId::Symbol
        | EId::Image
        | EId::ForeignObject
        | EId::Pattern
        | EId::Marker => {}
        _ => {
            node.remove_attribute(AId::Overflow);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, ToStringWithOptions};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_str($in_text).unwrap();
                remove_needless_attributes(&doc);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    test!(rm_clip_path_attrs_1,
"<svg>
    <clipPath>
        <rect width='10' fill='red'/>
    </clipPath>
</svg>",
"<svg>
    <clipPath>
        <rect width='10'/>
    </clipPath>
</svg>
");

    test!(rm_clip_path_attrs_2,
"<svg>
    <clipPath>
        <text fill='red' font-size='10'/>
    </clipPath>
</svg>",
"<svg>
    <clipPath>
        <text font-size='10'/>
    </clipPath>
</svg>
");

    test!(rm_rect_attrs_1,
"<svg>
    <rect dx='10' fill='#ff0000' r='5'/>
</svg>",
"<svg>
    <rect fill='#ff0000'/>
</svg>
");

    test!(rm_eb_1,
"<svg enable-background='new'>
    <g enable-background='new'>
        <rect enable-background='new'/>
    </g>
</svg>",
"<svg enable-background='new'>
    <g enable-background='new'>
        <rect/>
    </g>
</svg>
");

    test!(rm_fill_1,
"<svg>
    <rect fill='none' fill-rule='evenodd' fill-opacity='0.5'/>
</svg>",
"<svg>
    <rect fill='none'/>
</svg>
");

    test!(rm_fill_2,
"<svg>
    <rect fill='red' fill-rule='evenodd' fill-opacity='0'/>
</svg>",
"<svg>
    <rect fill='none'/>
</svg>
");

    test_eq!(keep_fill_1,
"<svg>
    <g fill='#ff0000' fill-rule='evenodd'>
        <rect fill='none'/>
    </g>
</svg>
");

    test!(rm_stroke_1,
"<svg>
    <rect stroke='none' stroke-width='5' stroke-linecap='square'/>
    <rect stroke='red' stroke-width='0' stroke-linecap='square'/>
    <rect stroke='red' stroke-opacity='0' stroke-linecap='square'/>
</svg>",
"<svg>
    <rect/>
    <rect/>
    <rect/>
</svg>
");

    test_eq!(keep_stroke_1,
"<svg>
    <g stroke='#ff0000'>
        <rect stroke='none'/>
    </g>
</svg>
");

    test!(rm_overflow_1,
"<svg overflow='scroll'>
    <rect overflow='visible'/>
</svg>",
"<svg overflow='scroll'>
    <rect/>
</svg>
");

}
