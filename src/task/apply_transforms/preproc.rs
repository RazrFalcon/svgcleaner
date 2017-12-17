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
    Node,
};
use svgdom::types::Transform;

use task::short::{EId, AId};
use super::utils;
use options::CleaningOptions;

// If group has transform and contains only valid elements
// we can apply the group's transform to children before applying transform to
// actual elements.
pub fn prepare_transforms(parent: &Node, recursive: bool, opt: &CleaningOptions) {
    let mut valid_elems: Vec<EId> = Vec::with_capacity(6);
    if opt.apply_transform_to_shapes {
        valid_elems.extend_from_slice(&[EId::Rect, EId::Circle, EId::Ellipse, EId::Line]);
    }

    if opt.paths_to_relative && opt.apply_transform_to_paths {
        valid_elems.push(EId::Path);
    }

    if valid_elems.is_empty() {
        // Nothing to do.
        return;
    }

    valid_elems.push(EId::G);

    let iter = parent.descendants().filter(|n|    n.is_tag_name(EId::G)
                                               && n.has_attribute(AId::Transform));

    for mut node in iter {
        if !utils::has_valid_transform(&node) || !utils::is_valid_attrs(&node) {
            continue;
        }

        // Check that all children is valid.
        let is_valid = node.children().svg().all(|(id, n)| {
            let is_valid_coords = if id == EId::Path || id == EId::G {
                true
            } else {
                utils::is_valid_coords(&n)
            };

               valid_elems.contains(&id)
            && utils::has_valid_transform(&n)
            && utils::is_valid_attrs(&n)
            && is_valid_coords
        });

        if is_valid {
            let ts = utils::get_ts(&node);
            apply_ts_to_children(&node, ts);
            node.remove_attribute(AId::Transform);
        }

        if !recursive {
            break;
        }
    }
}

fn apply_ts_to_children(node: &Node, ts: Transform) {
    for (_, mut child) in node.children().svg() {
        if child.has_attribute(AId::Transform) {
            // We should multiply transform matrices.
            let mut ts1 = ts;
            let ts2 = utils::get_ts(&child);
            ts1.append(&ts2);
            child.set_attribute((AId::Transform, ts1));
        } else {
            child.set_attribute((AId::Transform, ts));
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use options::CleaningOptions;
    use svgdom::{Document, ToStringWithOptions};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_str($in_text).unwrap();
                let svg = doc.svg_element().unwrap();

                let mut options = CleaningOptions::default();
                options.apply_transform_to_shapes = true;
                options.paths_to_relative = true;
                options.apply_transform_to_paths = true;

                prepare_transforms(&svg, true, &options);

                let mut opt = write_opt_for_tests!();
                opt.simplify_transform_matrices = true;
                assert_eq_text!(doc.to_string_with_opt(&opt), $out_text);
            }
        )
    }

    test!(apply_1,
"<svg>
    <g transform='translate(10 20)'>
        <rect transform='scale(2)'/>
        <path/>
    </g>
</svg>",
"<svg>
    <g>
        <rect transform='matrix(2 0 0 2 10 20)'/>
        <path transform='translate(10 20)'/>
    </g>
</svg>
");

    test_eq!(keep_1,
"<svg>
    <g transform='scale(10 20)'>
        <path d='M 10 20 L 30 40'/>
    </g>
    <g transform='scale(10 20)'>
        <rect d='M 10 20 L 30 40'/>
    </g>
</svg>
");

    test_eq!(keep_2,
"<svg>
    <linearGradient id='lg1'/>
    <g fill='url(#lg1)'>
        <rect/>
    </g>
    <g fill='url(#lg1)'>
        <path/>
    </g>
</svg>
");

    // Group should contain only supported children.
    test_eq!(keep_3,
"<svg>
    <g transform='scale(10 20)'>
        <g/>
        <rect/>
    </g>
</svg>
");

    // Both transforms should be valid.
    test_eq!(keep_4,
"<svg>
    <g transform='scale(10)'>
        <rect transform='scale(10 30)'/>
    </g>
</svg>
");

    // Non-SVG child.
    test!(keep_5,
"<svg>
    <g transform='scale(10)'>
        <rect transform='scale(10 30)'/>
        <test/>
    </g>
    <g transform='scale(10)'>
        <!-- test -->
    </g>
    <g transform='scale(10)'>
        Text
    </g>
</svg>",
"<svg>
    <g transform='scale(10)'>
        <rect transform='scale(10 30)'/>
        <test/>
    </g>
    <g>
        <!-- test -->
    </g>
    <g>Text</g>
</svg>
");
}
