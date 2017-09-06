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

pub fn remove_dupl_fe_gaussian_blur(doc: &Document) {
    let filter_attrs = [
        AId::X,
        AId::Y,
        AId::Width,
        AId::Height,
        AId::FilterUnits,
        AId::PrimitiveUnits,
        AId::FilterRes,
        AId::ColorInterpolationFilters,
    ];

    let fe_blur_attrs = [
        AId::StdDeviation,
    ];

    let mut nodes = Vec::new();

    for node in doc.descendants().filter(|n| n.is_tag_name(EId::Filter)) {
        // We support only filters with one child.
        if node.children().count() != 1 {
            continue;
        }

        // Filter should not be linked.
        if node.has_attribute(AId::XlinkHref) {
            continue;
        }

        // Check that child is 'feGaussianBlur'.
        let child = node.first_child().unwrap();
        if !child.is_tag_name(EId::FeGaussianBlur) {
            continue;
        }

        // Child should not be used.
        if child.is_used() {
            continue;
        }

        // We don't support this attributes.
        if child.has_attributes(&[AId::In, AId::Result]) {
            continue;
        }

        // Add to the list.
        nodes.push(node.clone());
    }

    super::rm_loop(&mut nodes, |node1, node2| {
        if !is_attrs_equal(node1, node2, &filter_attrs) {
            return false;
        }

        let child1 = node1.first_child().unwrap();
        let child2 = node2.first_child().unwrap();

        if !is_attrs_equal(&child1, &child2, &fe_blur_attrs) {
            return false;
        }

        true
    });
}

fn is_attrs_equal(node1: &Node, node2: &Node, attrs: &[AId]) -> bool {
    let attrs1 = node1.attributes();
    let attrs2 = node2.attributes();

    for aid in attrs.iter() {
        if !attrs1.contains(*aid) && !attrs2.contains(*aid) {
            continue;
        }

        if attrs1.contains(*aid) && attrs2.contains(*aid) {
            if attrs1.get_value(*aid).unwrap() != attrs2.get_value(*aid).unwrap() {
                return false;
            }
        } else {
            return false;
        }
    }

    true
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, ToStringWithOptions};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, remove_dupl_fe_gaussian_blur, $in_text, $out_text);
        )
    }

    test!(rm_1,
"<svg>
    <defs>
        <filter id='f1'>
            <feGaussianBlur stdDeviation='2'/>
        </filter>
        <filter id='f2'>
            <feGaussianBlur stdDeviation='2'/>
        </filter>
    </defs>
    <rect filter='url(#f2)'/>
</svg>",
"<svg>
    <defs>
        <filter id='f1'>
            <feGaussianBlur stdDeviation='2'/>
        </filter>
    </defs>
    <rect filter='url(#f1)'/>
</svg>
");

    test!(rm_2,
"<svg>
    <defs>
        <filter id='f1' x='5' y='5' width='10' height='10'>
            <feGaussianBlur stdDeviation='2'/>
        </filter>
        <filter id='f2' x='5' y='5' width='10' height='10'>
            <feGaussianBlur stdDeviation='2'/>
        </filter>
    </defs>
    <rect filter='url(#f2)'/>
</svg>",
"<svg>
    <defs>
        <filter id='f1' height='10' width='10' x='5' y='5'>
            <feGaussianBlur stdDeviation='2'/>
        </filter>
    </defs>
    <rect filter='url(#f1)'/>
</svg>
");

    // Keep when attrs is different.
    test_eq!(keep_1,
"<svg>
    <filter id='f1' height='10' width='10' x='50' y='5'>
        <feGaussianBlur stdDeviation='2'/>
    </filter>
    <filter id='f2' height='10' width='10' x='5' y='5'>
        <feGaussianBlur stdDeviation='2'/>
    </filter>
</svg>
");

    test_eq!(keep_2,
"<svg>
    <filter id='f1'>
        <feGaussianBlur stdDeviation='1'/>
    </filter>
    <filter id='f2'>
        <feGaussianBlur stdDeviation='2'/>
    </filter>
</svg>
");

    test_eq!(keep_3,
"<svg>
    <filter id='f1' xlink:href='#f2'>
        <feGaussianBlur stdDeviation='1'/>
    </filter>
    <filter id='f2'>
        <feGaussianBlur stdDeviation='1'/>
    </filter>
</svg>
");

}
