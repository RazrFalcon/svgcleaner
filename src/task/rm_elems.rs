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

use task::short::EId;

// TODO: to mod::utils
pub fn remove_element(doc: &mut Document, id: EId) {
    doc.drain(|n| n.is_tag_name(id));
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, ToStringWithOptions, ElementId};

    macro_rules! test {
        ($name:ident, $id:expr, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let mut doc = Document::from_str($in_text).unwrap();
                remove_element(&mut doc, $id);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    // TODO: this tests should be in svgdom

    test!(rm_1, ElementId::Title,
"<svg>
    <title/>
</svg>",
"<svg/>
");

    test!(rm_2, ElementId::Title,
"<svg>
    <title/>
    <title/>
    <rect/>
    <title/>
</svg>",
"<svg>
    <rect/>
</svg>
");

    test!(rm_3, ElementId::Title,
"<svg>
    <title>
        <title/>
        <rect/>
    </title>
    <rect/>
</svg>",
"<svg>
    <rect/>
</svg>
");
}
