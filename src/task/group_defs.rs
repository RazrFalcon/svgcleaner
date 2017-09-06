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
    AttributeType,
    Document,
    ElementType,
    Node,
};

use task::short::EId;

pub fn group_defs(doc: &mut Document) {
    // doc must contain 'svg' node, so we can safely unwrap.
    let mut svg = doc.svg_element().unwrap();

    // Create 'defs' node if it didn't exist already.
    let mut defs = match doc.descendants().filter(|n| n.is_tag_name(EId::Defs)).nth(0) {
        Some(n) => n,
        None => doc.create_element(EId::Defs),
    };

    // Make 'defs' a first child of the 'svg'.
    if svg.first_child() != Some(defs.clone()) {
        svg.prepend(&defs);
    }

    // Move all referenced elements to the main 'defs'.
    {
        let mut nodes = Vec::new();

        for (_, node) in doc.descendants().svg() {
            if node.is_referenced() {
                if let Some(parent) = node.parent() {
                    if parent != defs {
                        nodes.push(node.clone());
                    }
                }
            }
        }

        for n in &mut nodes {
            resolve_attrs(n);
            n.detach();
            defs.append(n);
        }
    }

    // Ungroup all existing 'defs', except main.
    {
        let mut nodes = Vec::new();
        for (_, node) in doc.descendants().svg() {
            if node.is_tag_name(EId::Defs) && node != defs {
                for child in node.children() {
                    nodes.push(child.clone());
                }
            }
        }

        for n in &mut nodes {
            n.detach();
            defs.append(n);
        }
    }

    // Remove empty 'defs', except main.
    {
        let mut nodes = Vec::new();
        for (_, node) in doc.descendants().svg() {
            if node.is_tag_name(EId::Defs) && node != defs {
                nodes.push(node.clone());
            }
        }

        for n in &mut nodes {
            // Unneeded defs already ungrouped and must be empty.
            debug_assert!(!n.has_children());
            n.remove();
        }
    }
}

// Graphical elements inside referenced elements inherits parent attributes,
// so if we want to move this elements to the 'defs' - we should resolve attributes too.
fn resolve_attrs(node: &Node) {
    match node.tag_id().unwrap() {
          EId::ClipPath
        | EId::Marker
        | EId::Mask
        | EId::Pattern
        | EId::Symbol => {
            let mut parent = node.clone();
            while let Some(p) = parent.parent() {
                let attrs = p.attributes();
                for (aid, attr) in attrs.iter_svg().filter(|&(_, a)| a.is_inheritable()) {
                    for mut child in node.children() {
                        if child.has_attribute(aid) {
                            continue;
                        }

                        child.set_attribute(attr.clone());
                    }
                }

                parent = p.clone();
            }
        }
        _ => {}
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, ToStringWithOptions};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, group_defs, $in_text, $out_text);
        )
    }

    // Add 'defs' to 'svg' node, not to first node.
    test!(create_defs_1,
"<!--comment--><svg/>",
"<!--comment-->
<svg>
    <defs/>
</svg>
");

    test!(move_defs_1,
"<svg>
    <altGlyphDef/>
    <clipPath/>
    <cursor/>
    <filter/>
    <linearGradient/>
    <marker/>
    <mask/>
    <pattern/>
    <radialGradient/>
    <symbol/>
    <rect/>
</svg>",
"<svg>
    <defs>
        <altGlyphDef/>
        <clipPath/>
        <cursor/>
        <filter/>
        <linearGradient/>
        <marker/>
        <mask/>
        <pattern/>
        <radialGradient/>
        <symbol/>
    </defs>
    <rect/>
</svg>
");

    // Complex. Recursive.
    test!(move_defs_2,
"<svg>
    <defs id='a'>
        <altGlyphDef/>
        <defs id='b'>
            <clipPath/>
        </defs>
        <defs id='c'>
            <defs id='d'>
                <cursor/>
                <filter/>
            </defs>
        </defs>
    </defs>
    <defs>
        <radialGradient/>
    </defs>
    <defs/>
</svg>",
"<svg>
    <defs id='a'>
        <altGlyphDef/>
        <clipPath/>
        <cursor/>
        <filter/>
        <radialGradient/>
    </defs>
</svg>
");

    // We should ungroup any nodes from 'defs'.
    test!(move_defs_3,
"<svg>
    <defs id='a'>
        <altGlyphDef/>
        <defs id='b'>
            <rect/>
        </defs>
    </defs>
</svg>",
"<svg>
    <defs id='a'>
        <altGlyphDef/>
        <rect/>
    </defs>
</svg>
");

    // Ungroupping should only work for direct 'defs' nodes.
    test!(move_defs_4,
"<svg>
    <defs id='a'>
        <defs id='b'>
            <clipPath>
                <rect/>
            </clipPath>
        </defs>
        <line>
            <animate/>
        </line>
    </defs>
</svg>",
"<svg>
    <defs id='a'>
        <line>
            <animate/>
        </line>
        <clipPath>
            <rect/>
        </clipPath>
    </defs>
</svg>
");

    test!(move_mask_1,
"<svg>
    <g fill='#ff0000'>
        <marker>
            <path/>
        </marker>
    </g>
</svg>",
"<svg>
    <defs>
        <marker>
            <path fill='#ff0000'/>
        </marker>
    </defs>
    <g fill='#ff0000'/>
</svg>
");

    test!(move_attrs_1,
"<svg>
    <g fill='#ff0000'>
        <marker>
            <path/>
            <path fill='#00ff00'/>
        </marker>
    </g>
</svg>",
"<svg>
    <defs>
        <marker>
            <path fill='#ff0000'/>
            <path fill='#00ff00'/>
        </marker>
    </defs>
    <g fill='#ff0000'/>
</svg>
");

    test!(move_attrs_2,
"<svg>
    <linearGradient id='lg1'/>
    <g fill='url(#lg1)'>
        <marker>
            <path/>
        </marker>
    </g>
</svg>",
"<svg>
    <defs>
        <linearGradient id='lg1'/>
        <marker>
            <path fill='url(#lg1)'/>
        </marker>
    </defs>
    <g fill='url(#lg1)'/>
</svg>
");

}
