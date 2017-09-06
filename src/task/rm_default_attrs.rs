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
    ValueId,
};
use svgdom::types::Length;

use task::short::{EId, AId, Unit};

// TODO: xml:space

pub fn remove_default_attributes(doc: &Document) {
    let mut rm_list = Vec::with_capacity(16);

    for (_, mut node) in doc.descendants().svg() {
        let tag_name = node.tag_id().unwrap();

        for (aid, attr) in node.attributes().iter_svg() {
            if attr.is_presentation() {
                if attr.check_is_default() {
                    if let Some(n) = node.parents().find(|n| n.has_attribute(aid)) {
                        if let Some(a) = n.attributes().get(aid) {
                            if !a.visible {
                                rm_list.push(aid);
                            }
                        }
                    } else {
                        rm_list.push(aid);
                    }
                } else if aid == AId::Overflow {
                    // The initial value for 'overflow' as defined in CSS2 is 'visible',
                    // and this applies also to the root 'svg' element;
                    // however, for child elements of an SVG document,
                    // SVG's user agent style sheet overrides this initial value and sets the
                    // 'overflow' property on elements that establish new viewports
                    // (e.g., 'svg' elements), 'pattern' elements and 'marker' elements
                    // to the value 'hidden'.
                    //
                    // https://www.w3.org/TR/SVG/masking.html#OverflowProperty

                    if node == doc.svg_element().unwrap() {
                        if let AttributeValue::PredefValue(id) = attr.value {
                            if id == ValueId::Visible {
                                rm_list.push(aid);
                            }
                        }
                    } else if let AttributeValue::PredefValue(id) = attr.value {
                        if id == ValueId::Hidden {
                            rm_list.push(aid);
                        }
                    }
                }
            } else if is_default(attr, tag_name) {
                // Check default values of an non-presentation attributes.
                rm_list.push(aid);
            }
        }

        {
            let mut attrs_mut = node.attributes_mut();
            for aid in &rm_list {
                // We only hide default attributes, because they still can be useful.
                attrs_mut.get_mut(*aid).unwrap().visible = false;
            }
        }

        rm_list.clear();
    }
}

fn is_default(attr: &Attribute, tag_name: EId) -> bool {
    // Process only popular and simple attributes.

    // TODO: this elements should be processed differently
    match tag_name {
          EId::LinearGradient
        | EId::RadialGradient => return false,
        _ => {}
    }

    // TODO: all attributes must be non-inheritable
    // TODO: theoretically, we can have any Unit. Investigate it.
    // TODO: use fuzzy_eq
    // TODO: add custom eq method to AttributeValue to simplify cmp

    match attr.id().unwrap() {
        AId::X | AId::Y => {
            if tag_name == EId::GlyphRef {
                return false;
            }

            if tag_name == EId::Mask || tag_name == EId::Filter {
                if attr.value == AttributeValue::from((-10.0, Unit::Percent)) {
                    return true;
                }
                return false;
            }

            match attr.value {
                AttributeValue::Length(l) => {
                    if l == Length::zero() {
                        return true;
                    }
                }
                AttributeValue::LengthList(ref list) => {
                    if tag_name == EId::Text && list.len() == 1 {
                        if list[0] == Length::zero() {
                            return true;
                        }
                    }
                }
                _ => {}
            }
        }
        AId::Width | AId::Height => {
            match tag_name {
                EId::Svg => {
                    if attr.value == AttributeValue::from((100.0, Unit::Percent)) {
                        return true;
                    }
                }
                EId::Pattern => {
                    if attr.value == AttributeValue::from((0.0, Unit::None)) {
                        return true;
                    }
                }
                EId::Mask | EId::Filter => {
                    if attr.value == AttributeValue::from((120.0, Unit::Percent)) {
                        return true;
                    }
                }
                _ => {}
            }
        }
        // TODO: this
        // temporary disabled since many render applications do not support this
        // AId::Rx | AId::Ry => {
        //     if tag_name == EId::Rect {
        //         if attr.value == AttributeValue::from((0.0, Unit::None)) {
        //             return true;
        //         }
        //     }
        // }
        AId::Cx | AId::Cy => {
            if tag_name == EId::Circle || tag_name == EId::Ellipse {
                if attr.value == AttributeValue::from((0.0, Unit::None)) {
                    return true;
                }
            }
        }
        AId::RefX | AId::RefY => {
            if tag_name == EId::Marker {
                if attr.value == AttributeValue::from((0.0, Unit::None)) {
                    return true;
                }
            }
        }
        AId::X1 | AId::Y1 | AId::X2 | AId::Y2 => {
            if tag_name == EId::Line {
                if attr.value == AttributeValue::from((0.0, Unit::None)) {
                    return true;
                }
            }
        }
        AId::StdDeviation => {
            if attr.value == AttributeValue::from((0.0, Unit::None)) {
                return true;
            }
        }
        AId::UnitsPerEm => {
            if let AttributeValue::String(ref s) = attr.value {
                if s == "1000" {
                    return true;
                }
            }
        }
        AId::Slope => {
            // There are two different 'slope': one for 'font-face'
            // and one for 'feFunc*'.
            if let AttributeValue::String(ref s) = attr.value {
                if tag_name == EId::FontFace && s == "0" {
                    return true;
                } else if s == "1" {
                    return true;
                }
            }
        }
          AId::ClipPathUnits
        | AId::MaskContentUnits
        | AId::PatternContentUnits
        | AId::PrimitiveUnits => {
            if let AttributeValue::PredefValue(v) = attr.value {
                if v == ValueId::UserSpaceOnUse {
                    return true;
                }
            }
        }
          AId::MaskUnits
        | AId::PatternUnits
        | AId::FilterUnits => {
            if let AttributeValue::PredefValue(v) = attr.value {
                if v == ValueId::ObjectBoundingBox {
                    return true;
                }
            }
        }
        _ => {}
    }

    false
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, ToStringWithOptions};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, remove_default_attributes, $in_text, $out_text);
        )
    }

    test!(rm_1,
"<svg>
    <rect fill='#000000'/>
</svg>",
"<svg>
    <rect/>
</svg>
");

    test!(keep_1,
"<svg fill='#ff0000'>
    <rect fill='#000000'/>
</svg>",
"<svg fill='#ff0000'>
    <rect fill='#000000'/>
</svg>
");

    test!(rm_svg_w_h,
"<svg width='100%' height='100%'/>",
"<svg/>
");

    // x, y attributes inside 'text' elements are not Length rather LengthList.
    test!(rm_len_list,
"<svg>
    <text x='0'/>
</svg>",
"<svg>
    <text/>
</svg>
");

    test!(rm_mask,
"<svg>
    <mask x='-10%' y='-10%' width='120%' height='120%'/>
</svg>",
"<svg>
    <mask/>
</svg>
");

    test!(rm_filter,
"<svg>
    <filter x='-10%' y='-10%' width='120%' height='120%'/>
</svg>",
"<svg>
    <filter/>
</svg>
");

    test!(rm_overflow_1,
"<svg overflow='visible'>
    <rect overflow='hidden'/>
    <svg overflow='visible'/>
</svg>",
"<svg>
    <rect/>
    <svg overflow='visible'/>
</svg>
");
}
