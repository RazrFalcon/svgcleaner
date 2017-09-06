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

pub fn remove_dupl_radial_gradients(doc: &Document) {
    let attrs = [
        AId::Cx,
        AId::Cy,
        AId::R,
        AId::Fx,
        AId::Fy,
        AId::GradientUnits,
        AId::SpreadMethod,
    ];

    let mut nodes = doc.descendants()
                       .filter(|n| n.is_tag_name(EId::RadialGradient))
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
                task::resolve_radial_gradient_attributes(&doc);
                remove_dupl_radial_gradients(&doc);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    test!(rm_1,
"<svg>
    <defs>
        <radialGradient id='rg1' cx='0' cy='0' fx='5' fy='5' r='10'>
            <stop offset='0' stop-color='#ff0000'/>
            <stop offset='1' stop-color='#0000ff'/>
        </radialGradient>
        <radialGradient id='rg2' cx='0' cy='0' fx='5' fy='5' r='10'>
            <stop offset='0' stop-color='#ff0000'/>
            <stop offset='1' stop-color='#0000ff'/>
        </radialGradient>
    </defs>
    <rect fill='url(#rg2)'/>
</svg>",
"<svg>
    <defs>
        <radialGradient id='rg1' cx='0' cy='0' fx='5' fy='5' r='10'>
            <stop offset='0' stop-color='#ff0000'/>
            <stop offset='1' stop-color='#0000ff'/>
        </radialGradient>
    </defs>
    <rect fill='url(#rg1)'/>
</svg>
");

    test!(rm_2,
"<svg>
    <defs>
        <radialGradient id='rg1' cx='0' cy='0' fx='5' fy='5' r='10'/>
        <radialGradient id='rg2' cx='0' cy='0' fx='5' fy='5' r='10'/>
    </defs>
    <rect fill='url(#rg2)'/>
</svg>",
"<svg>
    <defs>
        <radialGradient id='rg1' cx='0' cy='0' fx='5' fy='5' r='10'/>
    </defs>
    <rect fill='url(#rg1)'/>
</svg>
");

    test!(rm_3,
"<svg>
    <defs>
        <radialGradient id='rg1' cx='5' cy='5' fx='5' r='10'/>
        <radialGradient id='rg2' cx='5' cy='5' fy='5' r='10'/>
    </defs>
    <rect fill='url(#rg2)'/>
</svg>",
"<svg>
    <defs>
        <radialGradient id='rg1' cx='5' cy='5' fx='5' r='10'/>
    </defs>
    <rect fill='url(#rg1)'/>
</svg>
");

    test!(rm_4,
"<svg>
    <defs>
        <radialGradient id='rg1' cx='5' cy='5' fx='5' \
            gradientTransform='matrix(1 0 0 1 10 20)' r='10'/>
        <radialGradient id='rg2' cx='5' cy='5' fy='5' \
            gradientTransform='matrix(1 0 0 1 10 20)' r='10'/>
        <radialGradient id='rg3' cx='5' cy='5' fy='5' r='10'/>
    </defs>
    <rect fill='url(#rg2)'/>
</svg>",
"<svg>
    <defs>
        <radialGradient id='rg1' cx='5' cy='5' fx='5' \
            gradientTransform='translate(10 20)' r='10'/>
        <radialGradient id='rg3' cx='5' cy='5' fy='5' r='10'/>
    </defs>
    <rect fill='url(#rg1)'/>
</svg>
");

    test!(rm_5,
"<svg>
    <radialGradient id='rg1'/>
    <radialGradient id='rg2' xlink:href='#rg1'/>
</svg>",
"<svg>
    <radialGradient id='rg1'/>
</svg>
");

// TODO: this

//     test!(rm_6,
// "<svg>
//     <radialGradient id='rg2' xlink:href='#rg1'/>
//     <radialGradient id='rg1'/>
// </svg>",
// "<svg>
//     <radialGradient id='rg1'/>
// </svg>
// ");

    test!(rm_7,
"<svg>
    <radialGradient id='rg1'/>
    <radialGradient id='rg2' xlink:href='#rg1'/>
    <radialGradient id='rg3' xlink:href='#rg2'/>
</svg>",
"<svg>
    <radialGradient id='rg1'/>
</svg>
");

    test!(rm_8,
"<svg>
    <linearGradient id='lg1'/>
    <radialGradient id='rg1' xlink:href='#lg1'/>
    <radialGradient id='rg2' xlink:href='#lg1'/>
</svg>",
"<svg>
    <linearGradient id='lg1'/>
    <radialGradient id='rg1' xlink:href='#lg1'/>
</svg>
");
}
