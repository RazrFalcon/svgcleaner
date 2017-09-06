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
    Node,
};

use task::short::{EId, AId};

pub fn remove_dupl_linear_gradients(doc: &Document) {
    let attrs = [
        AId::X1,
        AId::Y1,
        AId::X2,
        AId::Y2,
        AId::GradientUnits,
        AId::SpreadMethod,
    ];

    let mut nodes = doc.descendants()
                       .filter(|n| n.is_tag_name(EId::LinearGradient))
                       .collect::<Vec<Node>>();

    super::rm_loop(&mut nodes, |node1, node2| {
        if !super::is_gradient_attrs_equal(node1, node2, &attrs) {
            return false;
        }

        if !super::is_equal_stops(node1, node2) {
            return false;
        }

        true
    });
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, ToStringWithOptions};
    use task;

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_str($in_text).unwrap();
                task::resolve_linear_gradient_attributes(&doc);
                remove_dupl_linear_gradients(&doc);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    test!(rm_1,
"<svg>
    <defs>
        <linearGradient id='lg1'>
            <stop offset='0' stop-color='#ff0000'/>
            <stop offset='1' stop-color='#0000ff'/>
        </linearGradient>
        <linearGradient id='lg2'>
            <stop offset='0' stop-color='#ff0000'/>
            <stop offset='1' stop-color='#0000ff'/>
        </linearGradient>
    </defs>
    <rect fill='url(#lg2)'/>
</svg>",
"<svg>
    <defs>
        <linearGradient id='lg1'>
            <stop offset='0' stop-color='#ff0000'/>
            <stop offset='1' stop-color='#0000ff'/>
        </linearGradient>
    </defs>
    <rect fill='url(#lg1)'/>
</svg>
");

    test!(rm_2,
"<svg>
    <defs>
        <linearGradient id='lg1'>
            <stop offset='0' stop-color='#ff0000'/>
            <stop offset='1' stop-color='#0000ff'/>
        </linearGradient>
        <linearGradient id='lg2'>
            <stop offset='0' stop-color='#ff0000'/>
            <stop offset='1' stop-color='#0000ff'/>
        </linearGradient>
        <linearGradient id='lg3'>
            <stop offset='0' stop-color='#ff0000'/>
            <stop offset='1' stop-color='#0000ff'/>
        </linearGradient>
    </defs>
    <rect fill='url(#lg2)'/>
    <rect fill='url(#lg3)'/>
</svg>",
"<svg>
    <defs>
        <linearGradient id='lg1'>
            <stop offset='0' stop-color='#ff0000'/>
            <stop offset='1' stop-color='#0000ff'/>
        </linearGradient>
    </defs>
    <rect fill='url(#lg1)'/>
    <rect fill='url(#lg1)'/>
</svg>
");

    // Different default attributes.
    test!(rm_3,
"<svg>
    <defs>
        <linearGradient id='lg1' x1='0%'/>
        <linearGradient id='lg2' x2='100%'/>
    </defs>
    <rect fill='url(#lg2)'/>
</svg>",
"<svg>
    <defs>
        <linearGradient id='lg1' x1='0%'/>
    </defs>
    <rect fill='url(#lg1)'/>
</svg>
");

    // No 'stop' elements.
    test!(rm_4,
"<svg>
    <defs>
        <linearGradient id='lg1'/>
        <linearGradient id='lg2'/>
    </defs>
    <rect fill='url(#lg2)'/>
</svg>",
"<svg>
    <defs>
        <linearGradient id='lg1'/>
    </defs>
    <rect fill='url(#lg1)'/>
</svg>
");

    test!(rm_5,
"<svg>
    <linearGradient id='lg1'>
        <stop/>
    </linearGradient>
    <linearGradient id='lg2' xlink:href='#lg1'/>
    <linearGradient id='lg3' xlink:href='#lg1'/>
    <rect fill='url(#lg2)'/>
    <rect fill='url(#lg3)'/>
</svg>",
"<svg>
    <linearGradient id='lg1'>
        <stop/>
    </linearGradient>
    <linearGradient id='lg2' xlink:href='#lg1'/>
    <rect fill='url(#lg2)'/>
    <rect fill='url(#lg2)'/>
</svg>
");

    test!(rm_6,
"<svg>
    <linearGradient id='lg1' xlink:href='#lg2'/>
    <linearGradient id='lg2'/>
</svg>",
"<svg>
    <linearGradient id='lg1'/>
</svg>
");
}
