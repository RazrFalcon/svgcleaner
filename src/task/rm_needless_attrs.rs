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

use svgdom::{Document, Node, Attribute, AttributeValue, ValueId};

// TODO: split to suboptions

pub fn remove_needless_attributes(doc: &Document) {
    for node in doc.descendants() {
        match node.tag_id().unwrap() {
            EId::ClipPath => process_clip_path(&node),
            EId::Rect => process_rect(&node),
            EId::Circle => process_circle(&node),
            EId::Ellipse => process_ellipse(&node),
            EId::Line => process_line(&node),
            EId::Polyline | EId::Polygon => process_poly(&node),
            _ => {}
        }

        // we can remove any 'color' attributes since they
        // already resolved in 'resolve_inherit', which makes them pointless
        node.remove_attribute(AId::Color);

        // `enable-background` is only applicable to container elements.
        // https://www.w3.org/TR/SVG/filters.html#EnableBackgroundProperty
        if !node.is_container() && node.has_attribute(AId::EnableBackground) {
            node.remove_attribute(AId::EnableBackground);
        }

        process_fill(&node);
        process_stroke(&node);
    }
}

fn process_clip_path(node: &Node) {
    for child in node.children() {
        if child.is_used() {
            continue;
        }

        // We can remove any fill and stroke based attrbiutes
        // since they does not impact rendering.
        // https://www.w3.org/TR/SVG/masking.html#EstablishingANewClippingPath
        child.attributes_mut().retain(|a| {
            !(a.is_fill() || a.is_stroke() || a.id == AId::Opacity)
        });
    }
}

fn process_rect(node: &Node) {
    // remove all non-rect attributes
    node.attributes_mut().retain(|a| {
           is_basic_shapes_attr(a)
        || a.id == AId::X
        || a.id == AId::Y
        || a.id == AId::Width
        || a.id == AId::Height
        || a.id == AId::Rx
        || a.id == AId::Ry
    });
}

fn process_circle(node: &Node) {
    // remove all non-circle attributes
    node.attributes_mut().retain(|a| {
           is_basic_shapes_attr(a)
        || a.id == AId::Cx
        || a.id == AId::Cy
        || a.id == AId::R
    });
}

fn process_ellipse(node: &Node) {
    // remove all non-ellipse attributes
    node.attributes_mut().retain(|a| {
           is_basic_shapes_attr(a)
        || a.id == AId::Cx
        || a.id == AId::Cy
        || a.id == AId::Rx
        || a.id == AId::Ry
    });
}

fn process_line(node: &Node) {
    // remove all non-line attributes
    node.attributes_mut().retain(|a| {
           is_basic_shapes_attr(a)
        || a.id == AId::X1
        || a.id == AId::Y1
        || a.id == AId::X2
        || a.id == AId::Y2
    });
}

fn process_poly(node: &Node) {
    // remove all non-polyline/polygon attributes
    node.attributes_mut().retain(|a| {
           is_basic_shapes_attr(a)
        || a.id == AId::Points
    });
}

fn is_basic_shapes_attr(a: &Attribute) -> bool {
    // list of common basic shapes attributes
    // https://www.w3.org/TR/SVG/shapes.html#RectElement

       a.is_conditional_processing()
    || a.is_core()
    || a.is_graphical_event()
    || a.is_presentation()
    || a.id == AId::Class
    || a.id == AId::Style
    || a.id == AId::ExternalResourcesRequired
    || a.id == AId::Transform
}

fn process_fill(node: &Node) {
    if !node.has_children() {
        // if 'fill' is disabled we can remove fill-based attributes
        if let Some(v) = node.attribute_value(AId::Fill) {
            if v == AttributeValue::PredefValue(ValueId::None) {
                node.remove_attribute(AId::FillRule);
                node.remove_attribute(AId::FillOpacity);
            }
        }

        // if 'fill' is invisible we can disable fill completely
        if let Some(v) = node.attribute_value(AId::FillOpacity) {
            if v == AttributeValue::Number(0.0) {
                node.set_attribute(AId::Fill, ValueId::None);
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

fn process_stroke(node: &Node) {
    fn is_invisible(node: &Node) -> bool {
        // skip nodes with marker, because it doesn't count opacity and stroke-width
        if node.has_attributes(&[AId::Marker, AId::MarkerStart, AId::MarkerMid, AId::MarkerEnd]) {
            return false;
        }

        // the stroke will not be drawn if stoke-width is 0
        if let Some(v) = node.attribute_value(AId::StrokeWidth) {
            if let AttributeValue::Length(l) = v {
                if l.num == 0.0 {
                    return true;
                }
            }
        }

        // the stroke will not be drawn if stoke-opacity is 0
        if let Some(v) = node.attribute_value(AId::StrokeOpacity) {
            if let AttributeValue::Number(num) = v {
                if num == 0.0 {
                    return true;
                }
            }
        }

        false
    }

    if !node.has_children() {
        if is_invisible(node) {
            // remove all stroke-based attributes if stroke is invisible
            node.remove_attribute(AId::Stroke);
            node.remove_attributes(STROKE_ATTRIBUTES);

            // if the parent element defines stroke - we must mark current element
            // with 'none' stroke
            if let Some(attr) = node.parent_attribute(AId::Stroke) {
                if attr.value != AttributeValue::PredefValue(ValueId::None) {
                    node.set_attribute(AId::Stroke, ValueId::None);
                }
            }
        } else {
            if let Some(v) = node.attribute_value(AId::Stroke) {
                if v == AttributeValue::PredefValue(ValueId::None) {
                    // remove all stroke-based attributes, except 'stroke' itself,
                    // if the stroke is 'none'
                    node.remove_attributes(STROKE_ATTRIBUTES);

                    // if the parent element doesn't define 'stroke' - we can remove it
                    // from the current element
                    if node.parent_attribute(AId::Stroke).is_none() {
                        node.remove_attribute(AId::Stroke);
                    }
                }
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, WriteToString};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_data($in_text).unwrap();
                remove_needless_attributes(&doc);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    macro_rules! test_eq {
        ($name:ident, $in_text:expr) => (
            test!($name, $in_text, String::from_utf8_lossy($in_text));
        )
    }

    test!(rm_clip_path_attrs_1,
b"<svg>
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
b"<svg>
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
b"<svg>
    <rect dx='10' fill='#ff0000' r='5'/>
</svg>",
"<svg>
    <rect fill='#ff0000'/>
</svg>
");

    test!(rm_eb_1,
b"<svg enable-background='new'>
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
b"<svg>
    <rect fill='none' fill-rule='evenodd' fill-opacity='0.5'/>
</svg>",
"<svg>
    <rect fill='none'/>
</svg>
");

    test!(rm_fill_2,
b"<svg>
    <rect fill='red' fill-rule='evenodd' fill-opacity='0'/>
</svg>",
"<svg>
    <rect fill='none'/>
</svg>
");


    test_eq!(keep_fill_1,
b"<svg>
    <g fill='#ff0000' fill-rule='evenodd'>
        <rect fill='none'/>
    </g>
</svg>
");

    test!(rm_stroke_1,
b"<svg>
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
b"<svg>
    <g stroke='#ff0000'>
        <rect stroke='none'/>
    </g>
</svg>
");

}
