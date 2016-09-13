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

use super::short::{EId};

use svgdom::{Document, TagName};

pub fn group_defs(doc: &Document) {
    // doc must contain 'svg' node, so we can safely unwrap
    let svg = doc.root().child_by_tag_id(EId::Svg).unwrap();

    let defs = match doc.root().child_by_tag_id(EId::Defs) {
        Some(n) => n,
        None => {
            // create 'defs' node if it didn't exist already
            let defs = doc.create_element(EId::Defs);
            svg.prepend(defs.clone());
            defs
        }
    };

    // move all referenced elements to main 'defs'
    // {
    //     let mut nodes = Vec::new();
    //     for node in doc.descendants() {
    //         if node.is_referenced() {
    //             if node.parent().unwrap() != defs {
    //                 nodes.push(node.clone());
    //             }
    //         }
    //     }

    //     for n in nodes {
    //         if n.is_tag_name(&TagName::Id(EId::Marker)) {
    //             prepare_node_attrs(&n);
    //         }

    //         if n.is_tag_name(&TagName::Id(EId::ClipPath)) {
    //             prepare_clip_path(&n);
    //         }

    //         n.detach();
    //         defs.append(&n);
    //     }
    // }

    // ungroup all existing 'defs', except main
    {
        let mut nodes = Vec::new();
        for node in doc.descendants() {
            if node.is_tag_name(&TagName::Id(EId::Defs)) && node != defs {
                for child in node.children() {
                    nodes.push(child.clone());
                }
            }
        }

        for n in nodes {
            n.detach();
            defs.append(&n);
        }
    }

    // remove empty 'defs', except main
    {
        let mut nodes = Vec::new();
        for node in doc.descendants() {
            if node.is_tag_name(&TagName::Id(EId::Defs)) && node != defs {
                nodes.push(node.clone());
            }
        }

        for n in nodes {
            // unneeded defs already ungrouped and must be empty
            debug_assert!(n.has_children() == false);
            n.remove();
        }
    }
}

// fn prepare_node_attrs(node: &Node) {
//     for a in P_ATTRIBUTES {
//         resolve_attr(node, a.clone());
//     }
// }

// fn prepare_clip_path(node: &Node) {
//     resolve_attr(node, AttributeId::ClipPath);
//     resolve_attr(node, AttributeId::ClipRule);
// }

// fn resolve_attr(node: &Node, id: AttributeId) {
//     if !node.has_attribute(id.clone()) {
//         match node.parent_attribute(id.clone()) {
//             Some(av) => {
//                 node.set_attribute(id.clone(), av);
//             }
//             None => {},
//         }
//     }
// }

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, WriteToString};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, group_defs, $in_text, $out_text);
        )
    }

    // add 'defs' to 'svg' node, not to first node
    test!(create_defs_1,
b"<!--comment--><svg/>",
"<!--comment-->
<svg>
    <defs/>
</svg>
");

//     #[test]
//     fn move_defs_1() {
//         // move all supported elements

//         let doc = Document::from_data(
// b"<svg>
//     <altGlyphDef/>
//     <clipPath/>
//     <cursor/>
//     <filter/>
//     <linearGradient/>
//     <marker/>
//     <mask/>
//     <pattern/>
//     <radialGradient/>
//     <symbol/>
//     <rect/>
// </svg>
// ").unwrap();

//         group_defs(&doc);

//         assert_eq!(doc_to_str_tests!(doc),
// "<svg>
//     <defs>
//         <altGlyphDef/>
//         <clipPath/>
//         <cursor/>
//         <filter/>
//         <linearGradient/>
//         <marker/>
//         <mask/>
//         <pattern/>
//         <radialGradient/>
//         <symbol/>
//     </defs>
//     <rect/>
// </svg>
// ");
//     }

    // complex, recursive
    test!(move_defs_2,
b"<svg>
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

        // we should ungroup any nodes from 'defs'
    test!(move_defs_3,
b"<svg>
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

    // ungroupping should only work for direct 'defs' nodes
    test!(move_defs_4,
b"<svg>
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

//     #[test]
//     fn move_mask_1() {
//         let doc = Document::from_data(
// b"<svg>
//     <g fill='#ff0000'>
//         <marker/>
//     </g>
// </svg>
// ").unwrap();

//         group_defs(&doc);

//         assert_eq!(doc_to_str_tests!(doc),
// "<svg>
//     <defs>
//         <marker fill='#ff0000'/>
//     </defs>
//     <g fill='#ff0000'/>
// </svg>
// ");
//     }

//     #[test]
//     fn move_clip_path_1() {
//         let doc = Document::from_data(
// b"<svg>
//     <g clip-path='none' clip-rule='nonzero'>
//         <clipPath/>
//     </g>
// </svg>
// ").unwrap();

//         group_defs(&doc);

//         assert_eq!(doc_to_str_tests!(doc),
// "<svg>
//     <defs>
//         <clipPath clip-path='none' clip-rule='nonzero'/>
//     </defs>
//     <g clip-path='none' clip-rule='nonzero'/>
// </svg>
// ");
//     }
}
