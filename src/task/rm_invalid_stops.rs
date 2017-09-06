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
    Document,
    ElementType,
};

use task::short::AId;
use task::utils;

pub fn remove_invalid_stops(doc: &Document) {
    let mut nodes = Vec::new();

    let iter = doc.descendants()
                  .filter(|n| n.is_gradient())
                  .filter(|n| n.has_children());
    for node in iter {
        let mut prev_child = node.first_child().unwrap();

        for child in node.children().skip(1) {
            {
                let attrs1 = prev_child.attributes();
                let attrs2 = child.attributes();

                if     attrs1.get_value(AId::Offset) == attrs2.get_value(AId::Offset)
                    && attrs1.get_value(AId::StopColor) == attrs2.get_value(AId::StopColor)
                    && attrs1.get_value(AId::StopOpacity) == attrs2.get_value(AId::StopOpacity) {
                    // If nothing changed - we can remove this 'stop'.
                    nodes.push(child.clone());
                }
            }

            prev_child = child.clone();
        }
    }

    utils::remove_nodes(&mut nodes);
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, ToStringWithOptions};
    use task;
    use task::fix_attrs;

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_str($in_text).unwrap();
                task::resolve_linear_gradient_attributes(&doc);
                task::resolve_radial_gradient_attributes(&doc);
                task::resolve_stop_attributes(&doc).unwrap();
                fix_attrs::fix_invalid_attributes(&doc);
                remove_invalid_stops(&doc);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    test!(rm_1,
"<svg>
    <linearGradient>
        <stop offset='0'/>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
</svg>",
"<svg>
    <linearGradient>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
</svg>
");

    test!(rm_2,
"<svg>
    <linearGradient>
        <stop offset='0'/>
        <stop offset='1'/>
        <stop offset='0.2'/>
    </linearGradient>
</svg>",
"<svg>
    <linearGradient>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
</svg>
");

    test!(rm_3,
"<svg>
    <linearGradient>
        <stop offset='0'/>
        <stop offset='0.5' stop-color='#ff0000'/>
        <stop offset='0.5' stop-color='#ff0000'/>
        <stop offset='1'/>
    </linearGradient>
</svg>",
"<svg>
    <linearGradient>
        <stop offset='0'/>
        <stop offset='0.5' stop-color='#ff0000'/>
        <stop offset='1'/>
    </linearGradient>
</svg>
");

    test_eq!(keep_1,
"<svg>
    <linearGradient>
        <stop offset='0'/>
        <stop offset='0.5' stop-color='#ff0000'/>
        <stop offset='0.5' stop-color='#00ff00'/>
        <stop offset='1'/>
    </linearGradient>
</svg>
");

    test_eq!(keep_2,
"<svg>
    <linearGradient>
        <stop offset='0'/>
        <stop offset='0.5' stop-opacity='0.6'/>
        <stop offset='0.5' stop-opacity='0.5'/>
        <stop offset='1'/>
    </linearGradient>
</svg>
");

    test_eq!(keep_3,
"<svg>
    <linearGradient>
        <stop offset='0'/>
        <stop offset='1' stop-color='#ff0000'/>
        <stop offset='1' stop-color='#00ff00'/>
    </linearGradient>
</svg>
");

}
