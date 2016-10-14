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

use super::short::{EId, AId, Unit};

use svgdom::{Document, Attribute, AttributeValue, ValueId};
use svgdom::types::{Length};

pub fn remove_default_attributes(doc: &Document) {
    let mut rm_list = Vec::with_capacity(16);

    for node in doc.descendants() {
        let tag_name = node.tag_id().unwrap();

        {
            let attrs = node.attributes();

            for (aid, attr) in attrs.iter_svg() {
                if attr.is_presentation() {
                    if attr.check_is_default() {
                        match node.parent_attribute(aid) {
                            Some(pattr) => {
                                if !pattr.visible {
                                    rm_list.push(aid);
                                }
                            }
                            None => {
                                rm_list.push(aid)
                            }
                        }
                    }
                } else if is_default(attr, tag_name) {
                    // check default values of an non-presentation attributes
                    rm_list.push(aid);
                }
            }
        }

        {
            let mut attrs_mut = node.attributes_mut();
            for aid in &rm_list {
                // we only hide default attributes, because they still can be useful
                attrs_mut.get_mut(*aid).unwrap().visible = false;
            }
        }

        rm_list.clear();
    }
}

fn is_default(attr: &Attribute, tag_name: EId) -> bool {
    // process only popular and simple attributes

    // TODO: this elements should be processed differently
    match tag_name {
          EId::LinearGradient
        | EId::RadialGradient => return false,
        _ => {}
    }

    // TODO: theoretically, we can have any Unit. Investigate it.

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
                    if l == Length::new(0.0, Unit::None) {
                        return true;
                    }
                }
                AttributeValue::LengthList(ref list) => {
                    if tag_name == EId::Text && list.len() == 1 {
                        if list[0] == Length::new(0.0, Unit::None) {
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
            if attr.value.as_string().unwrap() == "1000" {
                return true;
            }
        }
        AId::Slope => {
            // there are two different 'slope': one for 'font-face'
            // and one for 'feFunc*'
            let v = attr.value.as_string().unwrap();
            if tag_name == EId::FontFace && v == "0" {
                return true;
            } else if v == "1" {
                return true;
            }
        }
          AId::ClipPathUnits
        | AId::MaskContentUnits
        | AId::PatternContentUnits
        | AId::PrimitiveUnits => {
            if *attr.value.as_predef_value().unwrap() == ValueId::UserSpaceOnUse {
                return true;
            }
        }
          AId::MaskUnits
        | AId::PatternUnits
        | AId::FilterUnits => {
            if *attr.value.as_predef_value().unwrap() == ValueId::ObjectBoundingBox {
                return true;
            }
        }
        _ => {}
    }

    false
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, WriteToString};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, remove_default_attributes, $in_text, $out_text);
        )
    }

    test!(rm_1,
b"<svg>
    <rect fill='#000000'/>
</svg>",
"<svg>
    <rect/>
</svg>
");

    test!(keep_1,
b"<svg fill='#ff0000'>
    <rect fill='#000000'/>
</svg>",
"<svg fill='#ff0000'>
    <rect fill='#000000'/>
</svg>
");

    test!(rm_svg_w_h,
b"<svg width='100%' height='100%'/>",
"<svg/>
");

    // x, y attributes inside 'text' elements are not Length rather LengthList
    test!(rm_len_list,
b"<svg>
    <text x='0'/>
</svg>",
"<svg>
    <text/>
</svg>
");

    test!(rm_mask,
b"<svg>
    <mask x='-10%' y='-10%' width='120%' height='120%'/>
</svg>",
"<svg>
    <mask/>
</svg>
");

    test!(rm_filter,
b"<svg>
    <filter x='-10%' y='-10%' width='120%' height='120%'/>
</svg>",
"<svg>
    <filter/>
</svg>
");
}
