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
    ElementType,
    Node,
};

use task::short::{EId, AId};

pub fn regroup_gradient_stops(doc: &mut Document) {
    let mut nodes: Vec<Node> = doc.descendants()
        .filter(|n| n.is_gradient())
        .filter(|n| n.has_children())
        .filter(|n| !n.has_attribute(AId::XlinkHref))
        .collect();

    let mut is_changed = false;
    let mut join_nodes = Vec::new();

    // TODO: join with rm_dupl_defs::rm_loop
    let mut i1 = 0;
    while i1 < nodes.len() {
        let mut node1 = nodes[i1].clone();

        let mut i2 = i1 + 1;
        while i2 < nodes.len() {
            let node2 = nodes[i2].clone();
            i2 += 1;

            if super::rm_dupl_defs::is_equal_stops(&node1, &node2) {
                join_nodes.push(node2.clone());

                nodes.remove(i2 - 1);
                i2 -= 1;
            }
        }

        if !join_nodes.is_empty() {
            is_changed = true;

            let mut new_lg = doc.create_element(EId::LinearGradient);
            let new_id = gen_id(doc, "lg");
            new_lg.set_id(new_id);

            while node1.has_children() {
                let mut c = node1.first_child().unwrap();
                c.detach();
                new_lg.append(&c);
            }
            node1.set_attribute((AId::XlinkHref, new_lg.clone()));

            node1.insert_before(&new_lg);

            for jn in &mut join_nodes {
                while jn.has_children() {
                    let mut c = jn.first_child().unwrap();
                    c.remove();
                }
                jn.set_attribute((AId::XlinkHref, new_lg.clone()));
            }

            join_nodes.clear();
        }

        i1 += 1;
    }

    if is_changed {
        // We must resolve attributes for gradients created above.
        super::resolve_linear_gradient_attributes(doc);
    }
}

fn gen_id(doc: &Document, prefix: &str) -> String {
    let mut n = 1;

    let mut s = String::new();
    loop {
        s.clear();
        s.push_str(prefix);
        s.push_str(&n.to_string());

        // TODO: very slow
        if !doc.descendants().any(|n| *n.id() == s) {
            break;
        }

        n += 1;
    }

    s
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
                let mut doc = Document::from_str($in_text).unwrap();
                task::resolve_linear_gradient_attributes(&doc);
                task::resolve_radial_gradient_attributes(&doc);
                task::resolve_stop_attributes(&doc).unwrap();
                regroup_gradient_stops(&mut doc);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    test!(rm_1,
"<svg>
    <linearGradient id='lg1' x1='50'>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
    <linearGradient id='lg2' x1='100'>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
</svg>",
"<svg>
    <linearGradient id='lg3'>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
    <linearGradient id='lg1' x1='50' xlink:href='#lg3'/>
    <linearGradient id='lg2' x1='100' xlink:href='#lg3'/>
</svg>
");

    test!(rm_2,
"<svg>
    <linearGradient id='lg1' x1='50'>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
    <linearGradient id='lg3' x1='50'>
        <stop offset='0.5'/>
        <stop offset='1'/>
    </linearGradient>
    <linearGradient id='lg2' x1='100'>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
    <linearGradient id='lg4' x1='100'>
        <stop offset='0.5'/>
        <stop offset='1'/>
    </linearGradient>
</svg>",
"<svg>
    <linearGradient id='lg5'>
        <stop offset='0'/>
        <stop offset='1'/>
    </linearGradient>
    <linearGradient id='lg1' x1='50' xlink:href='#lg5'/>
    <linearGradient id='lg6'>
        <stop offset='0.5'/>
        <stop offset='1'/>
    </linearGradient>
    <linearGradient id='lg3' x1='50' xlink:href='#lg6'/>
    <linearGradient id='lg2' x1='100' xlink:href='#lg5'/>
    <linearGradient id='lg4' x1='100' xlink:href='#lg6'/>
</svg>
");

}
