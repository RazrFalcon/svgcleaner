/****************************************************************************
**
** SVG Cleaner could help you to clean up your SVG files
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

use svgdom::{Document, Node, AttributeValue};

// It's self defined list of the text attributes. There are no such list in the SVG spec.
static TEXT_ATTRIBUTES: &'static [AId] = &[
    AId::AlignmentBaseline,
    AId::BaselineShift,
    AId::Direction,
    AId::DominantBaseline,
    AId::Font,
    AId::FontFamily,
    AId::FontSize,
    AId::FontSizeAdjust,
    AId::FontStretch,
    AId::FontStyle,
    AId::FontVariant,
    AId::FontWeight,
    AId::GlyphOrientationHorizontal,
    AId::GlyphOrientationVertical,
    AId::Kerning,
    AId::LetterSpacing,
    AId::TextAnchor,
    AId::TextDecoration,
    AId::LineHeight,
    AId::WordSpacing,
    AId::WritingMode,
];

pub fn remove_text_attributes(doc: &Document) {
    // check doc for text nodes
    let has_text = doc.first_child().unwrap().has_text_children();

    for node in doc.descendants() {
        // remove all the text attributes if no text in the children nodes
        if !has_text && !has_em_ex_attributes(&node) {
            // skip any checks, except ex/em, if doc do not have any text nodes
            node.remove_attributes(&TEXT_ATTRIBUTES);
        } else if    !node.has_text_children()
                  && !node.is_tag_id(EId::FontFace)
                  && !node.child_by_tag_id(EId::Tref).is_some()
                  && !has_em_ex_attributes(&node)
                  && !is_linked_text(&node) {
            node.remove_attributes(&TEXT_ATTRIBUTES);
        }
    }
}

// an 'em' and an 'ex' units are depend on current font
fn has_em_ex_attributes(root: &Node) -> bool {
    // NOTE: actually, em/ex can be set in not parsed attributes, which will be ignored,
    //       but it's probably near to impossible

    for node in root.descendants() {
        let attrs = node.attributes();
        for attr in attrs.values() {
            match attr.value {
                AttributeValue::Length(ref len) => {
                    match len.unit {
                        Unit::Ex | Unit::Em => return true,
                        _ => {}
                    }
                }
                _ => {}
            }
        }
    }

    false
}

fn is_linked_text(node: &Node) -> bool {
    if node.is_tag_id(EId::Use) {
        // we use 'attributes()' method instead of 'attribute()',
        // because 'xlink:href' can contain base64 data, which will be expensive to copy
        let attrs = node.attributes();
        match attrs.get_value(AId::XlinkHref) {
            Some(value) => {
                match value {
                    &AttributeValue::Link(ref link) => {
                        return link.has_text_children();
                    }
                    _ => {}
                }
            }
            None => {}
        }
    }
    false
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, WriteToString};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, remove_text_attributes, $in_text, $out_text);
        )
    }

    macro_rules! test_eq {
        ($name:ident, $in_text:expr) => (
            test!($name, $in_text, String::from_utf8_lossy($in_text));
        )
    }

    test!(rm_text_1,
b"<svg font='Verdana'>
    <rect text-anchor='middle'/>
</svg>",
"<svg>
    <rect/>
</svg>
");

    // we can remove text attributes from the 'font-face' element
    // only when there is no text in whole doc
    test!(rm_text_2,
b"<svg>
    <font-face font-family='Verdana'/>
</svg>",
"<svg>
    <font-face/>
</svg>
");

    test!(keep_text_1,
b"<svg font='Verdana'>
    <g word-spacing='normal'>
        <text text-anchor='middle'>text</text>
    </g>
</svg>",
"<svg font='Verdana'>
    <g word-spacing='normal'>
        <text text-anchor='middle'>
            text
        </text>
    </g>
</svg>
");

    // keep font attributes on font-face, since it's global
    test_eq!(keep_text_2,
b"<svg>
    <font-face font-family='Verdana'/>
    <text>
        text
    </text>
</svg>
");

    test_eq!(keep_text_3,
b"<svg>
    <defs>
        <text id='hello'>
            Hello
        </text>
    </defs>
    <text font-family='Verdana'>
        <tref xlink:href='#hello'/>
    </text>
</svg>
");

    test_eq!(keep_text_4,
b"<svg>
    <g font-size='50'>
        <rect width='50ex'/>
    </g>
    <g font-size='50'>
        <rect width='50em'/>
    </g>
</svg>
");

    test_eq!(keep_text_5,
b"<svg>
    <defs>
        <text id='text'>
            Text
        </text>
    </defs>
    <use font-size='50' xlink:href='#text'/>
</svg>
");
}
