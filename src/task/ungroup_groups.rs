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

use svgdom::{Document, Node, AttributeValue};

pub fn ungroup_groups(doc: &Document) {
    let mut groups = Vec::with_capacity(16);

    // doc must contain 'svg' node, so we can safely unwrap
    let svg = doc.svg_element().unwrap();
    loop {
        _ungroup_groups(&svg, &mut groups);

        if groups.is_empty() {
            break;
        }

        while let Some(g) = groups.pop() {
            ungroup_group(&g);
            g.remove();
        }
    }
}

// Fill 'groups' vec with 'g' elements that should be removed.
// This method is recursive.
fn _ungroup_groups(root: &Node, groups: &mut Vec<Node>) {
    // We can't ungoup groups if they have one of the listed attribute.
    // Checkout 'painting-marker-02-f.svg' in 'W3C_SVG_11_TestSuite' for details.
    let invalid_attrs = [AId::Mask, AId::ClipPath, AId::Filter];

    // TODO: we should not ungroup groups with non-inheritable attributes.

    for node in root.children() {
        if node.is_tag_id(EId::G) {
            if !node.has_children() && !node.has_attribute(AId::Filter) {
                // Empty group without filter attribute.
                // Checkout 'filters-tile-01-b.svg' in 'W3C_SVG_11_TestSuite' for details.
                groups.push(node.clone());
                continue;
            }

            // group shouldn't be used
            if node.is_used() {
                continue;
            }

            // group shouldn't contain some attributes
            if node.has_attributes(&invalid_attrs) {
                continue;
            }

            // The 'clipPath' doesn't support 'g' elements, but some editors still insert them.
            // Correct render application will skip a 'g' element and all it's children,
            // so if we ungroup this group we will actually enable clipping.
            // Which will lead to broken image.
            // TODO: remove such groups completely
            if node.parent().unwrap().is_tag_id(EId::ClipPath) {
                continue;
            }

            if node.children().count() == 1 {
                let child = node.first_child().unwrap();
                // TODO: why child shouldn't be used?
                if !child.is_used() {
                    // group with only one child and neither group or child is used
                    groups.push(node.clone());
                    continue;
                }
            }

            // process group with many children

            // do not ungroup group which have 'switch' element as direct parent
            if node.parent().unwrap().is_tag_id(EId::Switch) {
                continue;
            }

            if node.attributes().is_empty() {
                // group without any attributes
                groups.push(node.clone());
                continue;
            }
        }

        if node.has_children() {
            _ungroup_groups(&node, groups);
        }
    }
}

fn ungroup_group(g: &Node) {
    for attr in g.attributes().iter() {
        for child in g.children() {
            if attr.id == AId::Opacity {
                if child.has_attribute(attr.id) {
                    // we can't just replace 'opacity' attribute,
                    // we should multiply it
                    let op1: f64 = *attr.value.as_number().unwrap();
                    let op2: f64 = *child.attribute_value(attr.id).unwrap()
                                         .as_number().unwrap();
                    child.set_attribute(attr.id, op1 * op2);
                    continue;
                }
            }

            if attr.id == AId::Transform {
                if child.has_attribute(attr.id) {
                    // we should multiply transform matrices
                    let mut t1 = *attr.value.as_transform().unwrap();

                    let a2 = child.attribute_value(attr.id).unwrap();
                    let t2 = a2.as_transform().unwrap();

                    t1.append(t2);
                    child.set_attribute(attr.id, t1);
                    continue;
                }
            }

            if attr.id == AId::Display {
                // display attribute has a priority during rendering, so we must
                // copy it even if a child has it already
                child.set_attribute(attr.id, attr.value.clone());
                continue;
            }

            if !child.has_attribute(attr.id) {
                match attr.value {
                    AttributeValue::Link(ref iri) | AttributeValue::FuncLink(ref iri) => {
                        // if it's fail - it's already a huge problem, so unwrap is harmless
                        child.set_link_attribute(attr.id, iri.clone()).unwrap();
                    }
                    _ => child.set_attribute(attr.id, attr.value.clone()),
                }
            }
        }
    }

    while g.has_children() {
        let c = g.last_child().unwrap();
        c.detach();
        g.insert_after(c);
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, WriteToString};
    use task::{group_defs, final_fixes, rm_unused_defs};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_data($in_text).unwrap();

                // prepare defs
                group_defs(&doc);

                // actual test
                ungroup_groups(&doc);

                // We must check that we moved linked elements correctly.
                // If we not, than referenced elements will be removed. Which is wrong.
                rm_unused_defs::remove_unused_defs(&doc);

                // removes `defs` element
                final_fixes(&doc);

                let mut opt = write_opt_for_tests!();
                opt.simplify_transform_matrices = true;
                assert_eq_text!(doc.to_string_with_opt(&opt), $out_text);
            }
        )
    }

    macro_rules! test_eq {
        ($name:ident, $in_text:expr) => (
            test!($name, $in_text, String::from_utf8_lossy($in_text));
        )
    }

    test!(rm_1,
b"<svg>
    <g/>
</svg>",
"<svg/>
");

    test!(rm_2,
b"<svg>
    <g>
        <g/>
    </g>
</svg>",
"<svg/>
");

    test!(ungroup_1,
b"<svg>
    <g>
        <rect/>
    </g>
</svg>",
"<svg>
    <rect/>
</svg>
");

    test!(ungroup_2,
b"<svg>
    <g>
        <g>
            <g>
                <g>
                    <rect/>
                </g>
            </g>
        </g>
    </g>
</svg>",
"<svg>
    <rect/>
</svg>
");

    // keep order
    test!(ungroup_3,
b"<svg>
    <g>
        <rect id='1'/>
        <rect id='2'/>
    </g>
</svg>",
"<svg>
    <rect id='1'/>
    <rect id='2'/>
</svg>
");

    test!(ungroup_4,
b"<svg>
    <g>
        <rect/>
        <g>
            <rect/>
        </g>
    </g>
</svg>",
"<svg>
    <rect/>
    <rect/>
</svg>
");

    test!(ungroup_5,
b"<svg>
    <switch>
        <foreignObject/>
        <g>
            <rect/>
        </g>
    </switch>
</svg>",
"<svg>
    <switch>
        <foreignObject/>
        <rect/>
    </switch>
</svg>
");

    // TODO: implement
//     test!(ungroup_5,
// b"<svg>
//     <g id='g1'/>
//     <use xlink:href='#g1'/>
// </svg>",
// b"<svg>
//     <use/>
// </svg>
// ");

    test_eq!(skip_ungroup_1,
b"<svg>
    <g fill='#ff0000'>
        <rect/>
        <rect/>
    </g>
</svg>
");

    test_eq!(skip_ungroup_2,
b"<svg>
    <defs>
        <filter id='f1'/>
    </defs>
    <g filter='url(#f1)'/>
</svg>
");

    test_eq!(skip_ungroup_3,
b"<svg>
    <g id='g1'>
        <rect/>
    </g>
    <use xlink:href='#g1'/>
</svg>
");

    test_eq!(skip_ungroup_4,
b"<svg>
    <defs>
        <clipPath id='clip1'/>
    </defs>
    <g clip-path='url(#clip1)'>
        <rect/>
    </g>
</svg>
");

    test_eq!(skip_ungroup_5,
b"<svg>
    <defs>
        <clipPath id='cp1'>
            <g transform='translate(5)'>
                <rect/>
            </g>
        </clipPath>
    </defs>
    <rect clip-path='url(#cp1)'/>
</svg>
");

    test_eq!(skip_ungroup_6,
b"<svg>
    <switch>
        <foreignObject/>
        <g>
            <rect/>
            <rect/>
        </g>
    </switch>
</svg>
");

    test!(ungroup_with_attrs_1,
b"<svg>
    <g fill='#ff0000'>
        <rect/>
    </g>
</svg>",
"<svg>
    <rect fill='#ff0000'/>
</svg>
");

    test!(ungroup_with_attrs_2,
b"<svg>
    <defs>
        <linearGradient id='lg1'/>
    </defs>
    <g fill='url(#lg1)'>
        <rect/>
    </g>
</svg>",
"<svg>
    <defs>
        <linearGradient id='lg1'/>
    </defs>
    <rect fill='url(#lg1)'/>
</svg>
");

    test!(ungroup_with_attrs_3,
b"<svg>
    <g display='none'>
        <rect display='inline'/>
    </g>
</svg>",
"<svg>
    <rect display='none'/>
</svg>
");

    test!(ungroup_with_opacity_1,
b"<svg>
    <g opacity='0.5'>
        <rect/>
    </g>
    <g opacity='0.5'>
        <rect opacity='0.5'/>
    </g>
</svg>",
"<svg>
    <rect opacity='0.5'/>
    <rect opacity='0.25'/>
</svg>
");

    test!(ungroup_with_transform_1,
b"<svg>
    <g transform='translate(10 20)'>
        <rect/>
    </g>
</svg>",
"<svg>
    <rect transform='translate(10 20)'/>
</svg>
");

    test!(ungroup_with_transform_2,
b"<svg>
    <g transform='translate(10 20)'>
        <rect transform='translate(20 30)'/>
    </g>
</svg>",
"<svg>
    <rect transform='translate(30 50)'/>
</svg>
");

    test!(ungroup_with_transform_3,
b"<svg>
    <g transform='translate(10 20)'>
        <g transform='translate(20 30)'>
            <rect/>
        </g>
    </g>
</svg>",
"<svg>
    <rect transform='translate(30 50)'/>
</svg>
");

    test!(ungroup_with_transform_4,
b"<svg>
    <g transform='translate(10 20)'>
        <g transform='translate(20 30)'>
            <g>
                <rect/>
            </g>
        </g>
        <g>
            <rect/>
        </g>
    </g>
</svg>",
"<svg>
    <g transform='translate(10 20)'>
        <rect transform='translate(20 30)'/>
        <rect/>
    </g>
</svg>
");
}
