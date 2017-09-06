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
    Document,
    Node,
    NodeType,
};

use task::short::{EId, AId, Unit};

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
    remove_xml_space(doc);

    // Check doc for text nodes.
    let has_text = doc.svg_element().unwrap()
                      .descendants().any(|n| n.node_type() == NodeType::Text);

    // We can remove text attributes from the 'font-face' element
    // only when there is no text in a whole doc.
    // Can't do it inside '_remove_text_attributes'.
    if !has_text {
        for (id, mut node) in doc.descendants().svg() {
            if id == EId::FontFace {
                node.remove_attributes(TEXT_ATTRIBUTES);
            }
        }
    }
}

// Returns 'true' if all children do not have any data than depend on text attributes.
fn _remove_text_attributes(parent: &Node) -> bool {
    // Process the tree recursively.

    // NOTE: not sure that it removes everything, at least it does not break anything.

    // Shorthand for no_text_data.
    let mut no_td = true;
    for mut node in parent.children() {
        // The 'line-height' property has no effect on text layout in SVG.
        //
        // https://www.w3.org/TR/SVG/text.html#FontProperty
        node.remove_attribute(AId::LineHeight);

        if !node.is_svg_element() {
            if node.node_type() == NodeType::Text {
                no_td = false;
            }

            continue;
        }

        if node.has_children() {
            // Go deeper.
            let can_rm = _remove_text_attributes(&node);
            if can_rm {
                if !node.is_tag_name(EId::FontFace) {
                    node.remove_attributes(TEXT_ATTRIBUTES);
                }
            } else {
                no_td = false;
            }
        } else {
            // Local version of the 'no_td'.

            // Only this parameters affect parent elements.
            let _no_td = !(   node.descendants().any(|n| n.node_type() == NodeType::Text)
                           || node.is_tag_name(EId::Tref)
                           || has_em_ex_attributes(&node));

            if    _no_td
               && !node.is_tag_name(EId::FontFace)
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
    // An 'em' and an 'ex' units are depend on current font.

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
    if node.is_tag_name(EId::Use) {
        // We use 'attributes()' method instead of 'attribute()',
        // because 'xlink:href' can contain base64 data, which will be expensive to copy.
        let attrs = node.attributes();
        if let Some(value) = attrs.get_value(AId::XlinkHref) {
            if let AttributeValue::Link(ref link) = *value {
                return link.descendants().any(|n| n.node_type() == NodeType::Text);
            }
        }
    }
    false
}

// Remove xml:space=preserve.
//
// Details: https://www.w3.org/TR/SVG/text.html#WhiteSpace
fn remove_xml_space(doc: &Document) {
    _remove_xml_space(&doc.root());
}

fn _remove_xml_space(parent: &Node) {
    // Processes the tree recursively.

    for mut node in parent.children() {
        // Check that node has attribute xml:space=preserve.
        //
        // xml:space=default will be removed by remove_default_attributes.
        let mut has_preserve = false;
        if let Some(&AttributeValue::String(ref s)) = node.attributes().get_value(AId::XmlSpace) {
            if s == "preserve" {
                has_preserve = true;
            }
        }

        if !has_preserve {
            // Go deeper.
            _remove_xml_space(&node);
            continue;
        }

        if node.has_children() {
            let mut has_spaces = false;
            for child in node.descendants() {
                if child.node_type() == NodeType::Text {
                    if is_text_contains_spaces(&child) {
                        has_spaces = true;
                        break;
                    }
                }
            }

            if !has_spaces {
                node.remove_attribute(AId::XmlSpace);
            }

            // Go deeper.
            _remove_xml_space(&node);
        } else {
            // If element doesn't have children than xml:space is useless
            // and we ca remove it.
            node.remove_attribute(AId::XmlSpace);
        }
    }
}

fn is_text_contains_spaces(text_node: &Node) -> bool {
    debug_assert_eq!(text_node.node_type(), NodeType::Text);

    let text = text_node.text();

    if text.is_empty() {
        return false
    }

    // 'trim' will remove leading and trailing spaces,
    // so it text is changed we had one and we can process such text.
    let trimmed = text.trim();
    if trimmed != *text {
        return true;
    }

    // Check that text contains pair of spaces.
    let mut has_spaces = false;
    let iter = trimmed.chars().zip(trimmed.chars().skip(1));
    for (c1, c2) in iter {
        if c1.is_whitespace() && c2.is_whitespace() {
            has_spaces = true;
            break;
        }
    }

    has_spaces
}

#[cfg(test)]
mod tests {
    use super::*;
    use super::remove_xml_space;
    use svgdom::{Document, ToStringWithOptions};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, remove_text_attributes, $in_text, $out_text);
        )
    }

    test!(rm_text_1,
"<svg font='Verdana'>
    <rect text-anchor='middle'/>
</svg>",
"<svg>
    <rect/>
</svg>
");

    // We can remove text attributes from the 'font-face' element
    // only when there is no text in a whole doc.
    test!(rm_text_2,
"<svg>
    <font-face font-family='Verdana'/>
</svg>",
"<svg>
    <font-face/>
</svg>
");

    test!(rm_text_3,
"<svg>
    <g font-family='Verdana'>
        <text text-anchor='middle'>text</text>
    </g>
    <g font-family='Verdana'>
        <rect/>
    </g>
</svg>",
"<svg>
    <g font-family='Verdana'>
        <text text-anchor='middle'>text</text>
    </g>
    <g>
        <rect/>
    </g>
</svg>
");

    test!(rm_text_4,
"<svg>
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
"<svg font='Verdana'>
    <g word-spacing='normal'>
        <text text-anchor='middle'>text</text>
    </g>
</svg>",
"<svg font='Verdana'>
    <g word-spacing='normal'>
        <text text-anchor='middle'>text</text>
    </g>
</svg>
");

    // Keep font attributes on font-face, since it's global.
    test_eq!(keep_text_2,
"<svg>
    <font-face font-family='Verdana'/>
    <text>text</text>
</svg>
");

    test!(keep_text_3,
"<svg>
    <defs>
        <text id='hello'>Hello</text>
    </defs>
    <text font-family='Verdana'>
        <tref xlink:href='#hello'/>
    </text>
</svg>",
"<svg>
    <defs>
        <text id='hello'>Hello</text>
    </defs>
    <text font-family='Verdana'>
        <tref xlink:href='#hello'/>
    </text>
</svg>
");

    test_eq!(keep_text_4,
"<svg>
    <g font-size='50'>
        <rect width='50ex'/>
    </g>
    <g font-size='50'>
        <rect width='50em'/>
    </g>
</svg>
");

    test_eq!(keep_text_5,
"<svg>
    <defs>
        <text id='text'>Text</text>
    </defs>
    <use font-size='50' xlink:href='#text'/>
</svg>
");

    // Do not take first node, take first element.
    test_eq!(keep_text_6,
"<!-- Comment -->
<svg>
    <text font-size='16'>text</text>
</svg>
");

    test_eq!(keep_text_7,
"<svg>
    <g font-size='10'>
        <rect width='10'/>
        <g>
            <rect width='10ex'/>
        </g>
        <rect width='10'/>
    </g>
</svg>
");

    macro_rules! test_space {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, remove_xml_space, $in_text, $out_text);
        )
    }

    macro_rules! test_space_eq {
        ($name:ident, $in_text:expr) => (
            test_space!($name, $in_text, $in_text);
        )
    }

    test_space!(space_preserve_1,
"<svg>
    <text xml:space='preserve'/>
    <text xml:space='preserve'></text>
    <text xml:space='preserve'>Text</text>
    <text xml:space='preserve'>Text Text</text>
</svg>",
"<svg>
    <text/>
    <text/>
    <text>Text</text>
    <text>Text Text</text>
</svg>
");

    test_space_eq!(space_preserve_keep_1,
"<svg>
    <text xml:space='preserve'> Text</text>
    <text xml:space='preserve'>Text </text>
    <text xml:space='preserve'> Text </text>
    <text xml:space='preserve'>Text  Text</text>
</svg>
");
}
