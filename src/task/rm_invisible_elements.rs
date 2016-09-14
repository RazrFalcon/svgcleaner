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

use svgdom::{Document, Node, AttributeValue};

// TODO: process display="none"

pub fn remove_invisible_elements(doc: &Document) {
    process_clip_path(doc);
}

// Remove invalid elements from 'clipPath' and if 'clipPath' is empty or became empty
// - remove it and all elements that became invalid or unused.
fn process_clip_path(doc: &Document) {
    let mut nodes = Vec::with_capacity(16);
    let mut clip_paths = Vec::with_capacity(16);

    // remove all invalid children
    for node in doc.descendants().filter(|n| n.is_tag_id(EId::ClipPath)) {
        for child in node.children() {
            if !is_valid_clip_path_elem(&child) {
                nodes.push(child.clone());
            }
        }

        while let Some(n) = nodes.pop() {
            n.remove();
        }

        if !node.has_children() {
            clip_paths.push(node.clone());
        }
    }

    // Remove empty clipPath's.
    // Note, that all elements that used this clip path alse became invisible,
    // so we can remove them as well.
    let is_rm_unused_defs = !clip_paths.is_empty();
    while let Some(n) = clip_paths.pop() {
        for link in n.linked_nodes() {
            link.remove();
        }
        n.remove();
    }

    if is_rm_unused_defs {
        super::remove_unused_defs(&doc);
    }
}

fn is_valid_clip_path_elem(node: &Node) -> bool {
    // https://www.w3.org/TR/SVG/masking.html#EstablishingANewClippingPath

    fn is_valid_shape(node: &Node) -> bool {
           node.is_basic_shape()
        || node.is_tag_id(EId::Path)
        || node.is_tag_id(EId::Text)
    }

    if node.is_tag_id(EId::Use) {
        if !node.has_attribute(AId::XlinkHref) {
            return false;
        }

        match node.attribute_value(AId::XlinkHref).unwrap() {
            AttributeValue::Link(link) => {
                return is_valid_shape(&link);
            }
            _ => {}
        }
    }

    is_valid_shape(node)
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, WriteToString};
    use task::{group_defs, final_fixes};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_data($in_text).unwrap();
                // we must prepare defs, because `remove_invisible_elements`
                // invokes `remove_unused_defs`
                group_defs(&doc);
                remove_invisible_elements(&doc);
                // removes `defs` element
                final_fixes(&doc);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    macro_rules! test_eq {
        ($name:ident, $in_text:expr) => (
            test!($name, $in_text, String::from_utf8_lossy($in_text));
        )
    }

    test!(rm_clip_path_children_1,
b"<svg>
    <clipPath>
        <g/>
        <rect/>
    </clipPath>
</svg>",
"<svg>
    <clipPath>
        <rect/>
    </clipPath>
</svg>
");

    test!(rm_clip_path_children_2,
b"<svg>
    <clipPath>
        <use/>
        <use xlink:href='#g1'/>
        <use xlink:href='#rect1'/>
    </clipPath>
    <rect id='rect1'/>
    <g id='g1'/>
</svg>",
"<svg>
    <clipPath>
        <use xlink:href='#rect1'/>
    </clipPath>
    <rect id='rect1'/>
    <g id='g1'/>
</svg>
");

    test!(rm_clip_path_1,
b"<svg>
    <clipPath id='cp1'/>
    <rect clip-path='url(#cp1)'/>
    <rect clip-path='url(#cp1)'/>
</svg>",
"<svg/>
");

    test!(rm_clip_path_2,
b"<svg>
    <linearGradient id='lg1'/>
    <clipPath id='cp1'/>
    <rect clip-path='url(#cp1)' fill='url(#lg1)'/>
</svg>",
"<svg/>
");

}
