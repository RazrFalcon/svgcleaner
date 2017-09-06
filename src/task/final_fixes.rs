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

use svgdom::Document;

use task::short::{EId, AId};

// TODO: is 'svg' element contains only one element and it's 'g' element - ungroup it

pub fn remove_empty_defs(doc: &Document) {
    // We don't remove all empty 'defs' elements, since it was already done by group_defs().
    // We only remove first 'defs', which is probably was created by us.

    // doc must contain 'svg' node, so we can safely unwrap.
    let svg = doc.svg_element().unwrap();
    for mut child in svg.children() {
        if child.is_tag_name(EId::Defs) && !child.has_children() {
            child.remove();
            break;
        }
    }
}

pub fn fix_xmlns_attribute(doc: &Document, rm_unused: bool) {
    // doc must contain 'svg' node, so we can safely unwrap.
    let mut svg = doc.svg_element().unwrap();

    let mut has_links = false;
    for (_, node) in doc.descendants().svg() {
        if node.is_used() {
            has_links = true;
            break;
        }

        if node.has_attribute(AId::XlinkHref) {
            has_links = true;
            break;
        }
    }

    let has_xlink = svg.has_attribute(AId::XmlnsXlink);

    if !has_links && rm_unused && has_xlink {
        // Remove if no links are used.
        svg.remove_attribute(AId::XmlnsXlink);
    } else if has_links && !has_xlink {
        // Set if needed.
        svg.set_attribute((AId::XmlnsXlink, "http://www.w3.org/1999/xlink"));
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, ToStringWithOptions};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, remove_empty_defs, $in_text, $out_text);
        )
    }

    test!(rm_defs_1,
"<svg>
    <defs/>
</svg>",
"<svg/>
");

    test!(rm_defs_2,
"<svg>
    <rect/>
    <!--comment-->
    <defs/>
</svg>",
"<svg>
    <rect/>
    <!--comment-->
</svg>
");

    test!(keep_1,
"<svg>
    <defs>
        <rect/>
    </defs>
</svg>",
"<svg>
    <defs>
        <rect/>
    </defs>
</svg>
");

    macro_rules! test_xmlns {
        ($name:ident, $in_text:expr, $out_text:expr, $rm_unused:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_str($in_text).unwrap();
                fix_xmlns_attribute(&doc, $rm_unused);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    test_xmlns!(xmlns_rm_1,
"<svg xmlns:xlink='http://www.w3.org/1999/xlink'/>",
"<svg/>
", true);

    test_xmlns!(xmlns_rm_2,
"<svg xmlns:xlink='http://www.w3.org/1999/xlink'/>",
"<svg xmlns:xlink='http://www.w3.org/1999/xlink'/>
", false);

    test_xmlns!(xmlns_keep_1,
"<svg xmlns:xlink='http://www.w3.org/1999/xlink'>
    <rect id='rect1'/>
    <use xlink:href='#rect1'/>
</svg>",
"<svg xmlns:xlink='http://www.w3.org/1999/xlink'>
    <rect id='rect1'/>
    <use xlink:href='#rect1'/>
</svg>
", true);

    test_xmlns!(xmlns_keep_2,
"<svg xmlns:xlink='http://www.w3.org/1999/xlink'>
    <font-face-uri xlink:href='../resources/SVGFreeSans.svg#ascii'/>
</svg>",
"<svg xmlns:xlink='http://www.w3.org/1999/xlink'>
    <font-face-uri xlink:href='../resources/SVGFreeSans.svg#ascii'/>
</svg>
", true);

    test_xmlns!(xmlns_fix_1,
"<svg>
    <rect id='rect1'/>
    <use xlink:href='#rect1'/>
</svg>",
"<svg xmlns:xlink='http://www.w3.org/1999/xlink'>
    <rect id='rect1'/>
    <use xlink:href='#rect1'/>
</svg>
", true);
}
