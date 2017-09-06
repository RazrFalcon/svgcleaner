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

use task::short::EId;

pub fn remove_unused_defs(doc: &Document) {
    // Unwrap is safe, because 'defs' already had been created in 'group_defs'.
    let mut defs = doc.descendants().filter(|n| n.is_tag_name(EId::Defs)).nth(0).unwrap();

    // Repeat until no unused nodes left.
    while remove_unused_defs_impl(&mut defs) { }
}

// Returns true if tree structure has been changed.
fn remove_unused_defs_impl(defs: &mut Node) -> bool {
    // TODO: understand how styles are propagates inside defs

    let mut mv_nodes = Vec::new();
    let mut rm_nodes = Vec::new();

    for node in defs.children() {
        if !node.is_used() && !is_font_node(&node) {
            // Nodes outside defs we have to move to main 'defs' node, not to parent,
            // because otherwise they became renderable.
            ungroup_children(&node, &mut mv_nodes, &mut rm_nodes);
        }
    }

    let is_any_changed = !mv_nodes.is_empty() || !rm_nodes.is_empty();


    if !is_any_changed {
        return false;
    }

    for node in mv_nodes {
        defs.append(&node);
    }

    for node in &mut rm_nodes {
        node.remove();
    }

    is_any_changed
}

fn is_font_node(node: &Node) -> bool {
    node.is_tag_name(EId::FontFace) || node.is_tag_name(EId::Font)
}

fn ungroup_children(node: &Node, mv_nodes: &mut Vec<Node>, rm_nodes: &mut Vec<Node>) {
    if node.has_children() {
        // Element can be unused, but elements in it can be,
        // so we need to move them to parent element before removing.
        for c in node.children() {
            mv_nodes.push(c.clone());
        }
    } else {
        rm_nodes.push(node.clone());
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, ToStringWithOptions};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, remove_unused_defs, $in_text, $out_text);
        )
    }

    test!(simple_1,
"<svg>
    <defs>
        <rect id='rect1'/>
        <rect/>
        <linearGradient/>
    </defs>
    <use xlink:href='#rect1'/>
</svg>",
"<svg>
    <defs>
        <rect id='rect1'/>
    </defs>
    <use xlink:href='#rect1'/>
</svg>
");

    test!(recursive_1,
"<svg>
    <defs>
        <linearGradient id='linearGradient1'>
            <stop/>
            <stop/>
        </linearGradient>
        <radialGradient xlink:href='#linearGradient1' id='radialGradient1'/>
    </defs>
</svg>",
"<svg>
    <defs/>
</svg>
");

    test!(correct_ungroup_1,
"<svg>
    <defs>
        <clipPath id='clipPath1'>
            <path id='path1'/>
        </clipPath>
    </defs>
    <use xlink:href='#path1'/>
</svg>",
"<svg>
    <defs>
        <path id='path1'/>
    </defs>
    <use xlink:href='#path1'/>
</svg>
");

    test_eq!(correct_ungroup_2,
"<svg>
    <defs>
        <clipPath id='clipPath1'>
            <path id='path1'/>
        </clipPath>
    </defs>
    <use clip-path='url(#clipPath1)' xlink:href='#path1'/>
</svg>
");

    test_eq!(correct_ungroup_3,
"<svg>
    <defs>
        <clipPath id='clipPath1'>
            <rect/>
            <g>
                <rect/>
            </g>
            <path/>
        </clipPath>
    </defs>
    <use clip-path='url(#clipPath1)'/>
</svg>
");

    test!(correct_ungroup_4,
"<svg>
    <defs>
        <clipPath>
            <g>
                <rect id='rect1'/>
            </g>
        </clipPath>
    </defs>
    <use xlink:href='#rect1'/>
</svg>",
"<svg>
    <defs>
        <rect id='rect1'/>
    </defs>
    <use xlink:href='#rect1'/>
</svg>
");

    test!(correct_ungroup_5,
"<svg>
    <defs>
        <foreignObject>
            <svg>
                <text id='hello'>
                    Hello
                </text>
            </svg>
        </foreignObject>
    </defs>
    <text>
        <tref xlink:href='#hello'/>
    </text>
</svg>",
"<svg>
    <defs>
        <text id='hello'>Hello</text>
    </defs>
    <text>
        <tref xlink:href='#hello'/>
    </text>
</svg>
");

    test!(correct_ungroup_6,
"<svg>
    <defs>
        <g>
            <rect id='rect1'/>
        </g>
    </defs>
    <use xlink:href='#rect1'/>
</svg>",
"<svg>
    <defs>
        <rect id='rect1'/>
    </defs>
    <use xlink:href='#rect1'/>
</svg>
");

    test!(keep_font_1,
"<svg>
    <defs>
        <font-face/>
        <font/>
    </defs>
</svg>",
"<svg>
    <defs>
        <font-face/>
        <font/>
    </defs>
</svg>
");

    test_eq!(keep_font_2,
"<svg>
    <defs>
        <font-face font-family='SVGFreeSansASCII' unicode-range='U+0-7F'>
            <font-face-src>
                <font-face-uri xlink:href='../resources/SVGFreeSans.svg#ascii'/>
            </font-face-src>
        </font-face>
    </defs>
</svg>
");
}
