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
    ElementType,
    Node,
    ValueId,
};
use svgdom::types::FuzzyEq;

use task::short::{EId, AId};
use task::utils;

// TODO: process mask element
// TODO: process visibility
// TODO: process feGaussianBlur with stdDeviation=0
// TODO: split to suboptions
// TODO: polyline/polygon without points
// TODO: remove elements with transform="matrix(0 0 0 0 0 0)"

pub fn remove_invisible_elements(doc: &mut Document) {
    let mut is_any_removed = false;
    process_display_attribute(doc, &mut is_any_removed);
    process_paths(doc, &mut is_any_removed);
    process_clip_paths(doc, &mut is_any_removed);
    process_empty_filter(doc, &mut is_any_removed);
    process_fe_color_matrix(doc);
    process_use(doc, &mut is_any_removed);
    process_gradients(doc, &mut is_any_removed);
    process_rect(doc, &mut is_any_removed);

    if is_any_removed {
        super::remove_unused_defs(doc);
    }
}

// Remove invalid elements from 'clipPath' and if 'clipPath' is empty or became empty
// - remove it and all elements that became invalid or unused.
fn process_clip_paths(doc: &Document, is_any_removed: &mut bool) {
    let mut nodes = Vec::with_capacity(16);
    let mut clip_paths = Vec::with_capacity(16);

    // Remove all invalid children.
    for node in doc.descendants().filter(|n| n.is_tag_name(EId::ClipPath)) {
        for child in node.children() {
            if !is_valid_clip_path_elem(&child) {
                nodes.push(child.clone());
            }
        }

        utils::remove_nodes(&mut nodes);

        if !node.has_children() {
            clip_paths.push(node.clone());
        }
    }

    if !clip_paths.is_empty() {
        *is_any_removed = true;
    }

    // Remove empty clipPath's.
    // Note, that all elements that uses this clip path also became invisible,
    // so we can remove them as well.
    while let Some(mut n) = clip_paths.pop() {
        for mut link in n.linked_nodes().collect::<Vec<Node>>() {
            link.remove();
        }
        n.remove();
    }
}

fn is_valid_clip_path_elem(node: &Node) -> bool {
    // https://www.w3.org/TR/SVG/masking.html#EstablishingANewClippingPath

    fn is_valid_shape(node: &Node) -> bool {
           node.is_basic_shape()
        || node.is_tag_name(EId::Path)
        || node.is_tag_name(EId::Text)
    }

    if node.is_tag_name(EId::Use) {
        if let Some(av) = node.attributes().get_value(AId::XlinkHref) {
            if let AttributeValue::Link(ref link) = *av {
                return is_valid_shape(link);
            }
        }
    }

    is_valid_shape(node)
}

// Paths with empty 'd' attribute are invisible and we can remove them.
fn process_paths(doc: &mut Document, is_any_removed: &mut bool) {
    fn is_invisible(node: &Node) -> bool {
        if let Some(&AttributeValue::Path(ref d)) = node.attributes().get_value(AId::D) {
            d.d.is_empty()
        } else {
            // Not set or invalid value type.
            true
        }
    }

    let c = doc.drain(|n| n.is_tag_name(EId::Path) && is_invisible(n));
    if c != 0 {
        *is_any_removed = true;
    }
}

// Remove elements with 'display:none'.
fn process_display_attribute(doc: &Document, is_any_removed: &mut bool) {
    let mut nodes = Vec::with_capacity(16);

    _process_display_attribute(&doc.root(), &mut nodes, is_any_removed);

    if !nodes.is_empty() {
        *is_any_removed = true;
    }

    utils::remove_nodes(&mut nodes);
}

fn _process_display_attribute(parent: &Node, nodes: &mut Vec<Node>, is_any_removed: &mut bool) {
    for (_, node) in parent.children().svg() {
        // If elements has attribute 'display:none' and this element is not used - we can remove it.
        let val = AttributeValue::PredefValue(ValueId::None);
        if node.attributes().get_value(AId::Display) == Some(&val) && !node.is_used() {
            // All children must be unused to.
            if !node.descendants().any(|n| n.is_used()) {
                // TODO: ungroup used elements and remove unused
                nodes.push(node.clone());
            }
        } else if node.has_children() {
            _process_display_attribute(&node, nodes, is_any_removed);
        }
    }
}

// Remove 'filter' elements without children.
fn process_empty_filter(doc: &Document, is_any_removed: &mut bool) {
    let mut nodes: Vec<Node> = doc.descendants()
                                  .filter(|n| n.is_tag_name(EId::Filter) && !n.has_children())
                                  .collect();

    if !nodes.is_empty() {
        *is_any_removed = true;
    }

    // Note, that all elements that uses this filter also became invisible,
    // so we can remove them as well.
    for n in &mut nodes {
        for mut link in n.linked_nodes().collect::<Vec<Node>>() {
            link.remove();
        }
        n.remove();
    }
}

// Remove feColorMatrix with default values.
fn process_fe_color_matrix(doc: &mut Document) {
    fn is_default_matrix(node: &Node) -> bool {
        if !node.is_tag_name(EId::Filter) {
            return false;
        }

        if node.children().count() != 1 {
            return false;
        }

        let child = node.first_child().unwrap();

        if !child.is_tag_name(EId::FeColorMatrix) {
            return false;
        }

        let attrs = child.attributes();

        // It's a very simple implementation since we do not parse matrix,
        // but it's enough to remove default feColorMatrix generated by Illustrator.
        if let Some(&AttributeValue::String(ref t)) = attrs.get_value(AId::Type) {
            if t == "matrix" {
                if let Some(&AttributeValue::String(ref values)) = attrs.get_value(AId::Values) {
                    if values == "1 0 0 0 0  0 1 0 0 0  0 0 1 0 0  0 0 0 1 0" {
                        // We remove the whole 'filter' elements.
                        return true;
                    }
                }
            }
        }

        false
    }

    doc.drain(is_default_matrix);
}

// 'use' element without 'xlink:href' attribute is pointless.
fn process_use(doc: &mut Document, is_any_removed: &mut bool) {
    let c = doc.drain(|n| n.is_tag_name(EId::Use) && !n.has_attribute(AId::XlinkHref));
    if c != 0 {
        *is_any_removed = true;
    }
}

fn process_gradients(doc: &Document, is_any_removed: &mut bool) {
    let mut nodes = Vec::with_capacity(16);

    {
        // Gradient without children and link to other gradient is pointless.
        let iter = doc.descendants()
                      .filter(|n| n.is_gradient())
                      .filter(|n| !n.has_children() && !n.has_attribute(AId::XlinkHref));

        for n in iter {
            for mut link in n.linked_nodes().collect::<Vec<Node>>() {
                while let Some(aid) = find_link_attribute(&link, &n) {
                    link.set_attribute((aid, ValueId::None));
                }
            }
            nodes.push(n.clone());
        }
    }

    // TODO: If 'x1' = 'x2' and 'y1' = 'y2', then the area to be painted will be painted as
    //       a single color using the color and opacity of the last gradient stop.

    {
        // 'If one stop is defined, then paint with the solid color fill using the color
        // defined for that gradient stop.'
        let iter = doc.descendants()
                      .filter(|n| n.is_gradient())
                      .filter(|n| n.children().count() == 1 && !n.has_attribute(AId::XlinkHref));

        for n in iter {
            let stop = n.first_child().unwrap();

            let color = match stop.attributes().get_value(AId::StopColor) {
                Some(&AttributeValue::Color(c)) => c,
                _ => unreachable!("attribute must be resolved"),
            };

            let opacity = match stop.attributes().get_value(AId::StopOpacity) {
                Some(&AttributeValue::Number(n)) => n,
                _ => unreachable!("attribute must be resolved"),
            };

            // Replace links with colors, but not in gradients,
            // because it will lead to 'xlink:href=#ffffff', which is wrong.
            for mut link in n.linked_nodes()
                             .filter(|n| !n.is_gradient())
                             .collect::<Vec<Node>>() {
                while let Some(aid) = find_link_attribute(&link, &n) {
                    link.set_attribute((aid, color));
                    if opacity.fuzzy_ne(&1.0) {
                        match aid {
                            AId::Fill => link.set_attribute((AId::FillOpacity, opacity)),
                            AId::Stroke => link.set_attribute((AId::StrokeOpacity, opacity)),
                            _ => {}
                        }
                    }
                }
            }
            nodes.push(n.clone());
        }
    }

    if !nodes.is_empty() {
        *is_any_removed = true;
    }

    utils::remove_nodes(&mut nodes);
}

fn find_link_attribute(node: &Node, link: &Node) -> Option<AId> {
    let attrs = node.attributes();

    for (aid, attr) in attrs.iter_svg() {
        match attr.value {
              AttributeValue::Link(ref n)
            | AttributeValue::FuncLink(ref n) => {
                if *n == *link {
                    return Some(aid);
                }
            }
            _ => {}
        }
    }

    None
}

// Remove rects with a zero size.
fn process_rect(doc: &mut Document, is_any_removed: &mut bool) {
    fn is_invisible(node: &Node) -> bool {
        if !node.is_tag_name(EId::Rect) {
            return false;
        }

        let attrs = node.attributes();

        if let Some(&AttributeValue::Length(len)) = attrs.get_value(AId::Width) {
            if len.num.is_fuzzy_zero() {
                return true;
            }
        }

        if let Some(&AttributeValue::Length(len)) = attrs.get_value(AId::Height) {
            if len.num.is_fuzzy_zero() {
                return true;
            }
        }

        false
    }

    let c = doc.drain(is_invisible);
    if c != 0 {
        *is_any_removed = true;
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, ToStringWithOptions};
    use task::{group_defs, remove_empty_defs};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let mut doc = Document::from_str($in_text).unwrap();
                // We must prepare defs, because 'remove_invisible_elements'
                // invokes 'remove_unused_defs'.
                group_defs(&mut doc);
                remove_invisible_elements(&mut doc);
                // Removes 'defs' element.
                remove_empty_defs(&mut doc);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    test!(rm_clip_path_children_1,
"<svg>
    <defs>
        <clipPath>
            <g/>
            <rect height='5' width='5'/>
        </clipPath>
    </defs>
</svg>",
"<svg>
    <defs>
        <clipPath>
            <rect height='5' width='5'/>
        </clipPath>
    </defs>
</svg>
");

    test!(rm_clip_path_children_2,
"<svg>
    <defs>
        <clipPath>
            <use/>
            <use xlink:href='#g1'/>
            <use xlink:href='#rect1'/>
        </clipPath>
    </defs>
    <rect id='rect1' height='5' width='5'/>
    <g id='g1'/>
</svg>",
"<svg>
    <defs>
        <clipPath>
            <use xlink:href='#rect1'/>
        </clipPath>
    </defs>
    <rect id='rect1' height='5' width='5'/>
    <g id='g1'/>
</svg>
");

    test!(rm_clip_path_1,
"<svg>
    <clipPath id='cp1'/>
    <rect clip-path='url(#cp1)' height='5' width='5'/>
    <rect clip-path='url(#cp1)' height='5' width='5'/>
</svg>",
"<svg/>
");

    test!(rm_clip_path_2,
"<svg>
    <linearGradient id='lg1'/>
    <clipPath id='cp1'/>
    <rect clip-path='url(#cp1)' fill='url(#lg1)' height='5' width='5'/>
</svg>",
"<svg/>
");

    test!(rm_clip_path_3,
"<svg>
    <clipPath>
        <rect display='none' height='5' width='5'/>
    </clipPath>
</svg>",
"<svg/>
");

    test!(rm_path_1,
"<svg>
    <path/>
</svg>",
"<svg/>
");

    test!(rm_path_2,
"<svg>
    <path d=''/>
</svg>",
"<svg/>
");

    test!(rm_path_3,
"<svg>
    <linearGradient id='lg1'/>
    <path d='' fill='url(#lg1)'/>
</svg>",
"<svg/>
");

    test!(rm_display_none_1,
"<svg>
    <path display='none'/>
</svg>",
"<svg/>
");

    test!(rm_display_none_2,
"<svg>
    <g display='none'>
        <rect height='5' width='5'/>
    </g>
</svg>",
"<svg/>
");

    test_eq!(skip_display_none_1,
"<svg>
    <g display='none'>
        <rect id='r1' height='5' width='5'/>
    </g>
    <use xlink:href='#r1'/>
</svg>
");

    test!(rm_filter_1,
"<svg>
    <filter/>
</svg>",
"<svg/>
");

    test!(rm_filter_2,
"<svg>
    <filter id='f1'/>
    <rect filter='url(#f1)' height='5' width='5'/>
</svg>",
"<svg/>
");

    test!(rm_use_1,
"<svg>
    <use/>
</svg>",
"<svg/>
");

    test!(rm_gradient_1,
"<svg>
    <linearGradient id='lg1'/>
    <rect fill='url(#lg1)' height='5' width='5'/>
    <rect stroke='url(#lg1)' height='5' width='5'/>
</svg>",
"<svg>
    <rect fill='none' height='5' width='5'/>
    <rect height='5' stroke='none' width='5'/>
</svg>
");

    test!(rm_gradient_2,
"<svg>
    <linearGradient id='lg1'>
        <stop offset='0.5' stop-color='#ff0000' stop-opacity='0.5'/>
    </linearGradient>
    <rect fill='url(#lg1)' stroke='url(#lg1)' height='5' width='5'/>
</svg>",
"<svg>
    <rect fill='#ff0000' fill-opacity='0.5' height='5' stroke='#ff0000' stroke-opacity='0.5' width='5'/>
</svg>
");

    test!(rm_gradient_3,
"<svg>
    <linearGradient id='lg1'>
        <stop offset='0.5' stop-color='#ff0000' stop-opacity='0.5'/>
    </linearGradient>
    <linearGradient id='lg2' xlink:href='#lg1'/>
    <rect fill='url(#lg1)' stroke='url(#lg1)' height='5' width='5'/>
</svg>",
"<svg>
    <rect fill='#ff0000' fill-opacity='0.5' height='5' stroke='#ff0000' stroke-opacity='0.5' width='5'/>
</svg>
");

    test!(rm_rect_1,
"<svg>
    <rect width='0' height='0'/>
    <rect width='10' height='0'/>
    <rect width='0' height='10'/>
</svg>",
"<svg/>
");

    test!(rm_fe_color_matrix_1,
"<svg>
    <filter id='filter1'>
        <feColorMatrix type='matrix' values='1 0 0 0 0  0 1 0 0 0  0 0 1 0 0  0 0 0 1 0'/>
    </filter>
    <rect filter='url(#filter1)' height='10' width='10'/>
</svg>",
"<svg>
    <rect height='10' width='10'/>
</svg>
");

}
