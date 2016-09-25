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

use svgdom::{Document, Node, NodeType, AttributeValue};

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
    AId::LineHeight,
    AId::TextAnchor,
    AId::TextDecoration,
    AId::WordSpacing,
    AId::WritingMode,
    AId::XmlSpace,
];

pub fn remove_text_attributes(doc: &Document) {
    _remove_text_attributes(&doc.root());

    // check doc for text nodes
    let has_text = doc.svg_element().unwrap().has_text_children();

    // We can remove text attributes from the 'font-face' element
    // only when there is no text in a whole doc.
    // Can't do it inside '_remove_text_attributes'.
    if !has_text {
        for node in doc.descendants() {
            if node.is_tag_id(EId::FontFace) {
                node.remove_attributes(TEXT_ATTRIBUTES);
            }
        }
    }
}

/// Returns `true` if all children do not have any data than depend on text attributes.
fn _remove_text_attributes(root: &Node) -> bool {
    // process tree recursively

    // NOTE: not sure that it removes everything, at least it does not break anything.

    // shorthand for no_text_data
    let mut no_td = true;
    for node in root.children_nodes() {
        if !node.is_svg_element() {
            if node.node_type() == NodeType::Text {
                no_td = false;
            }

            continue;
        }

        if node.has_children_nodes() {
            // go deeper
            let can_rm = _remove_text_attributes(&node);
            if can_rm {
                if !node.is_tag_id(EId::FontFace) {
                    node.remove_attributes(TEXT_ATTRIBUTES);
                }
            } else {
                no_td = false;
            }
        } else {
            // local version of the 'no_td'

            // only this parameters affect parent elements
            let _no_td = !(   node.has_text_children()
                           || node.is_tag_id(EId::Tref)
                           || has_em_ex_attributes(&node));

            if    _no_td
               && !node.is_tag_id(EId::FontFace)
               && !is_linked_text(&node) {
                node.remove_attributes(TEXT_ATTRIBUTES);
            }

            if !_no_td {
                no_td = false;
            }
        }
    }

    no_td
}

fn has_em_ex_attributes(node: &Node) -> bool {
    // an 'em' and an 'ex' units are depend on current font

    // NOTE: actually, em/ex can be set in not parsed attributes, which will be ignored,
    //       but it's probably near to impossible

    let attrs = node.attributes();
    for attr in attrs.iter() {
        if let AttributeValue::Length(ref len) = attr.value {
            match len.unit {
                Unit::Ex | Unit::Em => return true,
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
        if let Some(value) = attrs.get_value(AId::XlinkHref) {
            if let AttributeValue::Link(ref link) = *value {
                return link.has_text_children();
            }
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
    // only when there is no text in a whole doc
    test!(rm_text_2,
b"<svg>
    <font-face font-family='Verdana'/>
</svg>",
"<svg>
    <font-face/>
</svg>
");

    test!(rm_text_3,
b"<svg>
    <g font-family='Verdana'>
        <text text-anchor='middle'>
            text
        </text>
    </g>
    <g font-family='Verdana'>
        <rect/>
    </g>
</svg>",
"<svg>
    <g font-family='Verdana'>
        <text text-anchor='middle'>
            text
        </text>
    </g>
    <g>
        <rect/>
    </g>
</svg>
");

    test!(rm_text_4,
b"<svg>
    <g font-size='10'>
        <rect width='10ex'/>
    </g>
    <g font-size='10'>
        <rect width='10px'/>
    </g>
</svg>",
"<svg>
    <g font-size='10'>
        <rect width='10ex'/>
    </g>
    <g>
        <rect width='10px'/>
    </g>
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

    // do not take first node, take first element
    test_eq!(keep_text_6,
b"<!-- Comment -->
<svg>
    <text font-size='16'>
        text
    </text>
</svg>
");

    test_eq!(keep_text_7,
b"<svg>
    <g font-size='10'>
        <rect width='10'/>
        <g>
            <rect width='10ex'/>
        </g>
        <rect width='10'/>
    </g>
</svg>
");
}
