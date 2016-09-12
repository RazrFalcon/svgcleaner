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

use svgdom::{Document};

// TODO: rename
pub fn final_fixes(doc: &Document) {
    // Remove empty 'defs' element.
    // We don't remove all empty 'defs' elements, since it was already done by group_defs().
    // We only remove first 'defs', which is probably was created by us.
    let svg = doc.root().child_by_tag_id(EId::Svg).unwrap();
    for child in svg.children() {
        if child.is_tag_id(EId::Defs) && !child.has_children() {
            child.detach();
            break;
        }
    }
}

pub fn fix_xmlns_attribute(doc: &Document, rm_unused: bool) {
    let svg = doc.root().child_by_tag_id(EId::Svg).unwrap();

    let mut has_links = false;
    for node in doc.descendants() {
        if node.is_used() {
            has_links = true;
            break;
        }

        if node.has_attribute(AId::XlinkHref) {
            has_links = true;
            break;
        }
    }

    if !has_links && rm_unused && svg.has_attribute(AId::XmlnsXlink) {
        // remove if no links are used
        svg.remove_attribute(AId::XmlnsXlink);
    } else if has_links && !svg.has_attribute(AId::XmlnsXlink) {
        // set if needed
        svg.set_attribute(AId::XmlnsXlink, "http://www.w3.org/1999/xlink");
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, WriteToString};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, final_fixes, $in_text, $out_text);
        )
    }

    test!(rm_defs_1,
b"<svg>
    <defs/>
</svg>",
"<svg/>
");

    test!(rm_defs_2,
b"<svg>
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
b"<svg>
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
                let doc = Document::from_data($in_text).unwrap();
                fix_xmlns_attribute(&doc, $rm_unused);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    test_xmlns!(xmlns_rm_1,
b"<svg xmlns:xlink='http://www.w3.org/1999/xlink'/>",
"<svg/>
", true);

    test_xmlns!(xmlns_rm_2,
b"<svg xmlns:xlink='http://www.w3.org/1999/xlink'/>",
"<svg xmlns:xlink='http://www.w3.org/1999/xlink'/>
", false);

    test_xmlns!(xmlns_keep_1,
b"<svg xmlns:xlink='http://www.w3.org/1999/xlink'>
    <rect id='rect1'/>
    <use xlink:href='#rect1'/>
</svg>",
"<svg xmlns:xlink='http://www.w3.org/1999/xlink'>
    <rect id='rect1'/>
    <use xlink:href='#rect1'/>
</svg>
", true);

    test_xmlns!(xmlns_keep_2,
b"<svg xmlns:xlink='http://www.w3.org/1999/xlink'>
    <font-face-uri xlink:href='../resources/SVGFreeSans.svg#ascii'/>
</svg>",
"<svg xmlns:xlink='http://www.w3.org/1999/xlink'>
    <font-face-uri xlink:href='../resources/SVGFreeSans.svg#ascii'/>
</svg>
", true);

    test_xmlns!(xmlns_fix_1,
b"<svg>
    <rect id='rect1'/>
    <use xlink:href='#rect1'/>
</svg>",
"<svg xmlns:xlink='http://www.w3.org/1999/xlink'>
    <rect id='rect1'/>
    <use xlink:href='#rect1'/>
</svg>
", true);
}
