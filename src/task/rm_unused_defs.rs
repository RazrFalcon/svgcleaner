/****************************************************************************
**
** SVG Cleaner could help you to clean up your SVG files
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

use super::short::{EId};

use svgdom::{Document, Node};

pub fn remove_unused_defs(doc: &Document) {
    // repeat until no unused nodes left
    while remove_unused_defs_impl(doc) { }
}

// Returns true if tree structure has been changed.
fn remove_unused_defs_impl(doc: &Document) -> bool {
    // TODO: next release: font-face can probably be removed if no text elements are in the document
    // TODO: next release: font element should be referenced by font-family attribute,
    //                     if not - remove it
    // TODO: understand how styles are propagates inside defs

    let mut mv_nodes = Vec::new();
    let mut mv_to_defs_nodes = Vec::new();
    let mut rm_nodes = Vec::new();

    let mut iter = doc.descendants();
    while let Some(node) = iter.next() {
        if node.is_tag_id(EId::Defs) {
            for defs_child in node.children() {
                if !defs_child.is_used() && !is_font_node(&defs_child) {
                    ungroup_children(&defs_child, &mut mv_nodes, &mut rm_nodes);
                }
            }

            if node.has_children() {
                iter.skip_children();
            }
        } else if node.is_referenced() && !node.is_used() && !is_font_node(&node) {
            // nodes outside defs we have to move to main 'defs' node, not to parent,
            // because otherwise they became renderable
            ungroup_children(&node, &mut mv_to_defs_nodes, &mut rm_nodes);
            if node.has_children() {
                iter.skip_children();
            }
        }
    }

    let is_any_changed =    !mv_nodes.is_empty()
                         || !mv_to_defs_nodes.is_empty()
                         || !rm_nodes.is_empty();

    for node in mv_nodes {
        let defs = node.parent().unwrap().parent().unwrap();
        defs.append(&node);
    }

    let main_defs = doc.first_child().unwrap().child_by_tag_id(EId::Defs).unwrap();
    for node in mv_to_defs_nodes {
        main_defs.append(&node);
    }

    for node in rm_nodes {
        node.detach();
    }

    is_any_changed
}

fn is_font_node(node: &Node) -> bool {
    node.is_tag_id(EId::FontFace) || node.is_tag_id(EId::Font)
}

fn ungroup_children(node: &Node, mv_nodes: &mut Vec<Node>, rm_nodes: &mut Vec<Node>) {
    if node.has_children() {
        // element can be unused, but elements in it can be,
        // so we need to move them to parent element before removing
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
    use svgdom::{Document, WriteToString};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, remove_unused_defs, $in_text, $out_text);
        )
    }

    test!(simple_1,
b"<svg>
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
b"<svg>
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

    test!(outside_defs_1,
b"<svg>
    <defs/>
    <rect id='rect1'/>
    <use xlink:href='#rect1'/>
    <linearGradient id='linearGradient1'/>
</svg>",
"<svg>
    <defs/>
    <rect id='rect1'/>
    <use xlink:href='#rect1'/>
</svg>
");

    // we always have defs, because of group_defs()
    test!(outside_defs_2,
b"<svg>
    <defs/>
    <clipPath id='clipPath1'>
        <path id='path1'/>
    </clipPath>
    <use xlink:href='#path1'/>
</svg>",
"<svg>
    <defs>
        <path id='path1'/>
    </defs>
    <use xlink:href='#path1'/>
</svg>
");

    test!(correct_ungroup_1,
b"<svg>
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

    // ignore ungroup
    test!(correct_ungroup_2,
b"<svg>
    <defs>
        <clipPath id='clipPath1'>
            <path id='path1'/>
        </clipPath>
    </defs>
    <use clip-path='url(#clipPath1)' xlink:href='#path1'/>
</svg>",
"<svg>
    <defs>
        <clipPath id='clipPath1'>
            <path id='path1'/>
        </clipPath>
    </defs>
    <use clip-path='url(#clipPath1)' xlink:href='#path1'/>
</svg>
");

    // ignore ungroup
    test!(correct_ungroup_3,
b"<svg>
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
</svg>",
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
b"<svg>
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
b"<svg>
    <defs>
        <foreignObject>
            <svg>
                <text id='hello'>Hello</text>
            </svg>
        </foreignObject>
    </defs>
    <text>
        <tref xlink:href='#hello'/>
    </text>
</svg>",
"<svg>
    <defs>
        <text id='hello'>
            Hello
        </text>
    </defs>
    <text>
        <tref xlink:href='#hello'/>
    </text>
</svg>
");

    test!(correct_ungroup_6,
b"<svg>
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
b"<svg>
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

    test!(keep_font_2,
b"<svg>
    <defs>
        <font-face font-family='SVGFreeSansASCII' unicode-range='U+0-7F'>
            <font-face-src>
                <font-face-uri xlink:href='../resources/SVGFreeSans.svg#ascii'/>
            </font-face-src>
        </font-face>
    </defs>
</svg>",
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
