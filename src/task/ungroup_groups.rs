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
    AttributeValue,
    Document,
    Node,
};

use task::short::{EId, AId};
use task::apply_transforms;
use options::CleaningOptions;

pub fn ungroup_groups(doc: &Document, opt: &CleaningOptions) {
    let mut groups = Vec::with_capacity(16);

    // doc must contain 'svg' node, so we can safely unwrap.
    let svg = doc.svg_element().unwrap();
    loop {
        apply_transforms::prepare_transforms(doc, opt);

        _ungroup_groups(&svg, &mut groups, opt);

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
fn _ungroup_groups(parent: &Node, groups: &mut Vec<Node>, opt: &CleaningOptions) {
    // We can't ungroup groups if they have one of the listed attribute.
    // Checkout 'painting-marker-02-f.svg' in 'W3C_SVG_11_TestSuite' for details.
    let invalid_attrs = [AId::Mask, AId::ClipPath, AId::Filter];

    // TODO: we should not ungroup groups with non-inheritable attributes.

    for node in parent.children() {
        if node.is_tag_name(EId::G) {
            if !node.has_children() && !node.has_attribute(AId::Filter) {
                // Empty group without filter attribute.
                // Checkout 'filters-tile-01-b.svg' in 'W3C_SVG_11_TestSuite' for details.
                groups.push(node.clone());
                continue;
            }

            // Group shouldn't be used.
            if node.is_used() {
                continue;
            }

            // Group shouldn't contain some attributes.
            if node.has_attributes(&invalid_attrs) {
                continue;
            }

            // The 'clipPath' doesn't support 'g' elements, but some editors still insert them.
            // Correct render application will skip a 'g' element and all it's children,
            // so if we ungroup this group we will actually enable clipping.
            // Which will lead to broken image.
            // TODO: remove such groups completely
            if parent.is_tag_name(EId::ClipPath) {
                continue;
            }

            if node.children().count() == 1 {
                let child = node.first_child().unwrap();
                // TODO: why child shouldn't be used?
                if !child.is_used() {
                    // Group with only one child and neither group or child is used.
                    groups.push(node.clone());
                    continue;
                }
            }

            // Process group with many children.

            // Do not ungroup group which have 'switch' element as direct parent,
            // because it will break a 'switch'.
            if parent.is_tag_name(EId::Switch) {
                continue;
            }

            if node.attributes().is_empty() {
                // Group without any attributes.
                groups.push(node.clone());
                continue;
            }

            // If group has only a transform attribute
            // and if all children elements contain transform
            // and none of the children is 'use'.
            //
            // example: oxygen/edit-find-mail.svg
            if node.has_attribute(AId::Transform) && node.attributes().len() == 1 {
                let is_ok = node.children().all(|n| {
                    n.has_attribute(AId::Transform) && !n.is_tag_name(EId::Use)
                });

                if is_ok {
                    groups.push(node.clone());
                    continue;
                }
            }
        }

        if node.has_children() {
            _ungroup_groups(&node, groups, opt);
        }
    }
}

fn ungroup_group(g: &Node) {
    for (aid, attr) in g.attributes().iter_svg() {
        for child in g.children() {
            if aid == AId::Opacity {
                if child.has_attribute(aid) {
                    // We can't just replace 'opacity' attribute,
                    // we should multiply it.

                    let op1 = match attr.value {
                        AttributeValue::Number(v) => v,
                        _ => unreachable!("must be resolved"),
                    };

                    let op2 = match child.attributes().get_value(aid).cloned() {
                        Some(AttributeValue::Number(v)) => v,
                        _ => unreachable!("must be resolved"),
                    };

                    child.set_attribute((aid, op1 * op2));
                    continue;
                }
            }

            if aid == AId::Transform {
                if child.has_attribute(aid) {
                    // We should multiply transform matrices.
                    if let AttributeValue::Transform(mut t1) = attr.value {
                        let mut attrs = child.attributes_mut();
                        let av = attrs.get_value_mut(AId::Transform);
                        if let Some(&mut AttributeValue::Transform(ref mut ts)) = av {
                            t1.append(&ts);
                            *ts = t1;
                        }
                    }

                    continue;
                }
            }

            if aid == AId::Display {
                // Display attribute has a priority during rendering, so we must
                // copy it even if a child has it already.
                child.set_attribute((aid, attr.value.clone()));
                continue;
            }

            if !child.has_attribute(aid) {
                match attr.value {
                    AttributeValue::Link(ref iri) | AttributeValue::FuncLink(ref iri) => {
                        // If it's fail - it's already a huge problem, so unwrap is harmless.
                        child.set_attribute((aid, iri.clone()));
                    }
                    _ => child.set_attribute((aid, attr.value.clone())),
                }
            }
        }
    }

    while g.has_children() {
        let c = g.last_child().unwrap();
        c.detach();
        g.insert_after(&c);
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, WriteToString};
    use task::{group_defs, remove_empty_defs, rm_unused_defs};
    use options::CleaningOptions;

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_str($in_text).unwrap();

                // Prepare defs.
                group_defs(&doc);

                let mut options = CleaningOptions::default();
                options.apply_transform_to_shapes = true;
                options.paths_to_relative = true;
                options.apply_transform_to_paths = true;

                // Actual test.
                ungroup_groups(&doc, &options);

                // We must check that we moved linked elements correctly.
                // If we not, than referenced elements will be removed. Which is wrong.
                rm_unused_defs::remove_unused_defs(&doc);

                // Removes 'defs' element.
                remove_empty_defs(&doc);

                let mut opt = write_opt_for_tests!();
                opt.simplify_transform_matrices = true;
                assert_eq_text!(doc.to_string_with_opt(&opt), $out_text);
            }
        )
    }

    test!(rm_1,
"<svg>
    <g/>
</svg>",
"<svg/>
");

    test!(rm_2,
"<svg>
    <g>
        <g/>
    </g>
</svg>",
"<svg/>
");

    test!(ungroup_1,
"<svg>
    <g>
        <rect/>
    </g>
</svg>",
"<svg>
    <rect/>
</svg>
");

    test!(ungroup_2,
"<svg>
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

    // Keep order.
    test!(ungroup_3,
"<svg>
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
"<svg>
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
"<svg>
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
// "<svg>
//     <g id='g1'/>
//     <use xlink:href='#g1'/>
// </svg>",
// "<svg>
//     <use/>
// </svg>
// ");

    test_eq!(skip_ungroup_1,
"<svg>
    <g fill='#ff0000'>
        <rect/>
        <rect/>
    </g>
</svg>
");

    test_eq!(skip_ungroup_2,
"<svg>
    <defs>
        <filter id='f1'/>
    </defs>
    <g filter='url(#f1)'/>
</svg>
");

    test_eq!(skip_ungroup_3,
"<svg>
    <g id='g1'>
        <rect/>
    </g>
    <use xlink:href='#g1'/>
</svg>
");

    test_eq!(skip_ungroup_4,
"<svg>
    <defs>
        <clipPath id='clip1'/>
    </defs>
    <g clip-path='url(#clip1)'>
        <rect/>
    </g>
</svg>
");

    test!(skip_ungroup_5,
"<svg>
    <defs>
        <clipPath id='cp1'>
            <g transform='translate(5)'>
                <rect/>
            </g>
        </clipPath>
    </defs>
    <rect clip-path='url(#cp1)'/>
</svg>",
"<svg>
    <defs>
        <clipPath id='cp1'>
            <g>
                <rect transform='translate(5)'/>
            </g>
        </clipPath>
    </defs>
    <rect clip-path='url(#cp1)'/>
</svg>
");

    test_eq!(skip_ungroup_6,
"<svg>
    <switch>
        <foreignObject/>
        <g>
            <rect/>
            <rect/>
        </g>
    </switch>
</svg>
");

    test_eq!(skip_ungroup_7,
"<svg>
    <g transform='translate(10 20)'>
        <rect transform='translate(10 20)'/>
        <use transform='translate(10 20)'/>
    </g>
</svg>
");

    test!(ungroup_with_attrs_1,
"<svg>
    <g fill='#ff0000'>
        <rect/>
    </g>
</svg>",
"<svg>
    <rect fill='#ff0000'/>
</svg>
");

    test!(ungroup_with_attrs_2,
"<svg>
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
"<svg>
    <g display='none'>
        <rect display='inline'/>
    </g>
</svg>",
"<svg>
    <rect display='none'/>
</svg>
");

    test!(ungroup_with_opacity_1,
"<svg>
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
"<svg>
    <g transform='translate(10 20)'>
        <rect/>
    </g>
</svg>",
"<svg>
    <rect transform='translate(10 20)'/>
</svg>
");

    test!(ungroup_with_transform_2,
"<svg>
    <g transform='translate(10 20)'>
        <rect transform='translate(20 30)'/>
    </g>
</svg>",
"<svg>
    <rect transform='translate(30 50)'/>
</svg>
");

    test!(ungroup_with_transform_3,
"<svg>
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
"<svg>
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
    <rect transform='translate(30 50)'/>
    <rect transform='translate(10 20)'/>
</svg>
");

    // Ungroup group with transform when all children also has a transform
    // but only when group has only one attribute: transform.
    test!(ungroup_with_transform_5,
"<svg>
    <g transform='translate(10 20)'>
        <rect transform='translate(10 20)'/>
        <rect transform='translate(10 20)'/>
    </g>
    <g fill='#ff0000' transform='translate(10 20)'>
        <rect transform='translate(10 20)'/>
        <rect transform='translate(10 20)'/>
    </g>
</svg>",
"<svg>
    <rect transform='translate(20 40)'/>
    <rect transform='translate(20 40)'/>
    <g fill='#ff0000'>
        <rect transform='translate(20 40)'/>
        <rect transform='translate(20 40)'/>
    </g>
</svg>
");

    // Ungroup because transform will be applied to rect
    // only when 'Options::apply_transform_to_shapes' is enabled.
    test!(ungroup_with_transform_6,
"<svg>
    <g transform='translate(10 20)'>
        <rect/>
        <rect/>
    </g>
</svg>",
"<svg>
    <rect transform='translate(10 20)'/>
    <rect transform='translate(10 20)'/>
</svg>
");
}
