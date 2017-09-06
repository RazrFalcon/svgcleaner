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

use std::fmt;
use std::cmp;
use std::ops::Range;

use svgdom::{
    Attribute,
    AttributeType,
    AttributeValue,
    Document,
    Indent,
    Node,
    WriteOptions,
};

use task::short::{EId, AId};

// TODO: optimize, since Table is basically Vec<(Vec,Vec)>, which is not very efficient
struct Table {
    d: Vec<TableRow>,
}

struct TableRow {
    attributes: Vec<Attribute>,
    flags: Vec<bool>,
}

impl Table {
    fn new() -> Table {
        Table { d: Vec::new() }
    }

    fn append(&mut self, attr: &Attribute, col_count: usize) {
        let mut v2 = Vec::new();
        v2.resize(col_count, false);

        self.d.push(TableRow {
            attributes: vec![attr.clone()],
            flags: v2,
        });
    }

    // Removes unneeded rows.
    fn simplify(&mut self, min_nodes_count: usize) {
        // Table should be already defined/filled.
        debug_assert!(!self.d.is_empty());
        debug_assert!(min_nodes_count == 2 || min_nodes_count == 3);
        debug_assert!(self.d[0].flags.len() >= min_nodes_count);

        // We group only N or more elements,
        // so rows with less than N flags are useless.
        self.d.retain(|x| {
            x.count_flags() >= min_nodes_count
        });

        // Rows should contain flags that repeats at least N times.
        // |*-*| -> |---|
        // |**-**| -> |-----|
        // |*-*-*-*| -> |-------|
        // |***-*-*| -> |***----|
        // etc.

        for d in &mut self.d {
            let f = &mut d.flags;

            let mut idx = 0;
            let mut count = 0;
            while idx < f.len() {
                if f[idx] == true {
                    count += 1;
                } else {
                    if count == 1 {
                        f[idx - 1] = false;
                    } else if count == 2 {
                        f[idx - 2] = false;
                        f[idx - 1] = false;
                    }

                    count = 0;
                }

                idx += 1;
            }
        }

        // Remove again.
        self.d.retain(|x| {
            x.count_flags() >= min_nodes_count
        });
    }

    // Sort rows by the longest continuous range of set flags.
    fn sort(&mut self) {
        self.d.sort_by(|a, b| {
            let ac = a.longest();
            let bc = b.longest();
            bc.cmp(&ac)
        });
    }

    // Join rows.
    fn join(&mut self) {
        // Replace:
        // a |-***-|
        // b |-***-|
        //
        // with
        // a,b |-***-|

        let mut idx = 0;
        while idx < self.d.len() - 1 {
            if self.d[idx].flags == self.d[idx + 1].flags {
                let attr = self.d[idx + 1].attributes[0].clone();
                self.d[idx].attributes.push(attr);
                self.d.remove(idx + 1);

                continue;
            }

            idx += 1;
        }
    }
}

impl TableRow {
    // Returns an amount of set/true flags.
    fn count_flags(&self) -> usize {
        self.flags.iter().filter(|f| **f).count()
    }

    // Returns a length of the longest continuous range of set flags.
    fn longest(&self) -> usize {
        let mut max_count = 0;
        let mut count = 0;
        for f in &self.flags {
            if *f == true {
                count += 1;
            } else {
                max_count = cmp::max(max_count, count);
                count = 0;
            }
        }
        max_count = cmp::max(max_count, count);

        max_count
    }

    // Returns the longest continuous range of set flags.
    //
    // # Example
    //
    // `|-***-*****--|` -> (5, 10)
    fn longest_range(&self) -> Range<usize> {
        let mut list = Vec::new();

        let mut start = None;
        let mut end = 0;
        for (idx, f) in self.flags.iter().enumerate() {
            if *f == true {
                if start.is_none() {
                    start = Some(idx);
                }

                end = idx;
            } else if let Some(s) = start {
                list.push(s..end);
                start = None;
                end = 0;
            }
        }
        if let Some(s) = start {
            list.push(s..end);
        }

        list.sort_by(|a, b| b.len().cmp(&a.len()));

        list[0].clone()
    }
}

impl fmt::Debug for Table {
    // Prints something like:
    // fill="#ff0000"   |*-*|
    // stroke="#00ff00" |***|
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        if self.d.is_empty() {
            return write!(f, "empty table");
        }

        let mut lines: Vec<String> = Vec::new();
        for d in &self.d {
            let mut s = String::new();

            for a in &d.attributes {
                s.push_str(&format!("{}", a));
                s.push(',');
            }
            s.pop();

            lines.push(s);
        }

        let mut s = String::new();
        let max_len = lines.iter().max_by_key(|s| s.len()).unwrap().len();

        for (idx, line) in lines.iter().enumerate() {
            s.push_str(line);

            // Indent.
            for _ in 0..max_len - line.len() {
                s.push(' ');
            }

            s.push_str(" |");
            for f in &self.d[idx].flags {
                s.push(if *f { '*' } else { '-' });
            }
            s.push_str("|\n");
        }
        s.pop();

        write!(f, "{}", s)
    }
}

#[cfg(test)]
mod table_tests {
    use svgdom::{Attribute, AttributeId as AId};
    use super::{Table, TableRow};

    fn row_from_str(text: &str) -> Vec<bool> {
        let mut vec = Vec::new();
        for c in text.chars() {
            match c {
                '*' => vec.push(true),
                '-' => vec.push(false),
                _ => unreachable!(),
            }
        }

        vec
    }

    macro_rules! test_all {
        ($name:ident, [$( $in_flags:expr ),*] , $result_text:expr) => (
            #[test]
            fn $name() {
                let mut table_in = Table::new();

                $(
                    table_in.d.push(TableRow {
                        attributes: vec![Attribute::new(AId::Fill, "red")],
                        flags: row_from_str($in_flags),
                    });
                )*

                table_in.simplify(3);

                if table_in.d.is_empty() {
                    assert_eq!(format!("{:?}", table_in), $result_text);
                    return;
                }

                table_in.sort();
                table_in.join();

                assert_eq!(format!("{:?}", table_in), $result_text);
            }
        )
    }

    test_all!(rm_1, ["*-*-*-*-"], "empty table");
    test_all!(rm_2, ["**-**-"], "empty table");
    test_all!(rm_3, ["*-**-*"], "empty table");
    test_all!(rm_4, ["-**-**"], "empty table");

    // We use dummy attributes since we care only about flags.

    test_all!(basic_1,
        ["*****"],
        "fill=\"red\" |*****|");

    test_all!(join_1,
        ["*****", "*****"],
        "fill=\"red\",fill=\"red\" |*****|");

    test_all!(join_2,
        ["-***-", "-***-"],
        "fill=\"red\",fill=\"red\" |-***-|");

    test_all!(join_3,
        ["*****", "*****", "-***-", "-***-"],
        "fill=\"red\",fill=\"red\" |*****|\n\
         fill=\"red\",fill=\"red\" |-***-|");

    test_all!(sort_1,
        ["-****", "*****", "-***-", "-**--"],
        "fill=\"red\" |*****|\n\
         fill=\"red\" |-****|\n\
         fill=\"red\" |-***-|");

    // We care only about longest range, not about a number of set flags.
    test_all!(sort_2,
        ["*****-******", "**********--"],
        "fill=\"red\" |**********--|\n\
         fill=\"red\" |*****-******|");
}

pub fn group_by_style(doc: &Document, opt: &WriteOptions) {
    _group_by_style(&doc.svg_element().unwrap(), opt);
}

fn _group_by_style(parent: &Node, opt: &WriteOptions) {
    let mut node_list = Vec::with_capacity(16);

    // We can reuse an existing group only if all children are valid.
    let mut is_all_children = true;

    // Collect nodes.
    // TODO: currently we ignore non-SVG elements, which is bad
    for (_, node) in parent.children().svg() {
        // If 'defs' node occurred - skip it and reset the list.

        // Node can't be used, because a 'use' retrieves only element's attributes
        // and not parent ones. So if we move attributes to the group - 'use' element
        // will be rendered incorrectly.
        if node.is_tag_name(EId::Defs) || node.is_used() {
            is_all_children = false;
            node_list.clear();
            continue;
        }

        // Recursive processing.
        if node.has_children() {
            _group_by_style(&node, opt);
        }

        node_list.push(node);
    }

    if !parent.is_tag_name(EId::Svg) && !parent.is_tag_name(EId::G) {
        return;
    }


    // Grouping of 3 and more children are always efficient.
    //
    // 57B
    // <rect fill='red'/>
    // <rect fill='red'/>
    // <rect fill='red'/>
    //
    // 56B
    // <g>
    //     <rect/>
    //     <rect/>
    //     <rect/>
    // </g>
    //
    // And with smaller indent even more efficient.
    //
    // But the grouping of 2 children only efficient with zero or none indent.
    //
    // 38B
    // <rect fill='red'/>
    // <rect fill='red'/>
    //
    // 36B
    // <g fill='red'>
    // <rect/>
    // <rect/>
    // </g>
    //
    // 33B
    // <g fill='red'><rect/><rect/></g>
    //
    // So we join groups with 2 children only when a parent element is already a group
    // or when indent is zero on none.
    let min_nodes_count = {
        let is_small_indent =    opt.indent == Indent::None
                              || opt.indent == Indent::Spaces(0);

        let is_small =    parent.is_tag_name(EId::Svg)
                       || parent.is_tag_name(EId::G)
                       || is_small_indent;

        if is_small { 2 } else { 3 }
    };
    if node_list.len() < min_nodes_count {
        return;
    }

    let nodes_count = node_list.len();
    let mut table = Table::new();

    // Fill table with attributes.
    for node in &node_list {
        let attrs = node.attributes();

        for (aid, attr) in attrs.iter_svg() {
            if !attr.visible {
                continue;
            }

            if aid == AId::Transform {
                // We can't group a transform if a node or the parent has linked elements.

                if !super::apply_transforms::utils::is_valid_attrs(node) {
                    continue;
                }

                if !super::apply_transforms::utils::is_valid_attrs(parent) {
                    continue;
                }
            } else if !attr.is_inheritable() {
                continue;
            }

            // Append only unique attributes.
            if !table.d.iter().any(|x| x.attributes[0] == *attr) {
                table.append(attr, nodes_count);
            }
        }
    }

    if table.d.is_empty() {
        return;
    }

    // Set attributes flags inside the table.
    for d in &mut table.d {
        for (idx, node) in node_list.iter().enumerate() {
            let attrs = node.attributes();
            if attrs.iter().any(|a| *a == d.attributes[0]) {
                d.flags[idx] = true;
            }
        }
    }

    table.simplify(min_nodes_count);

    // 'simplify' can remove rows, so:
    if table.d.is_empty() {
        return;
    }

    table.sort();
    table.join();

    // If first row contain all flags set, aka:
    // fill='red' |*******|
    // so we can group all selected elements.
    if table.d[0].count_flags() == nodes_count {
        // If parent node is 'g' - use it,
        // it not - create new one.
        let is_valid_parent = parent.is_tag_name(EId::Svg) || parent.is_tag_name(EId::G);
        let mut g_node = if is_valid_parent && is_all_children {
            parent.clone()
        } else {
            let g_node = parent.document().create_element(EId::G);
            node_list[0].insert_before(&g_node);
            g_node
        };

        let len = node_list.len();
        move_nodes(&table.d[0].attributes, &mut g_node, &mut node_list, 0..len);

        // Remove first row.
        table.d.remove(0);
    }

    if table.d.is_empty() {
        return;
    }

    // Process row that is partially filled, aka:
    // fill='red' |-*****---|
    // so we can group selected elements to the subgroup.
    // We can do this only once, because we can't group nodes that
    // already been had been moved to subgroup.
    {
        let d = &table.d[0];

        // Get the longest range on nodes.
        let range = d.longest_range();

        // Do the same as in previous block.
        let mut g_node = parent.document().create_element(EId::G);
        node_list[range.start].insert_before(&g_node);

        move_nodes(&d.attributes, &mut g_node, &mut node_list, range);
    }

    // TODO: process rows with multiple ranges, aka
    // |*****--*****|
    // Currently only first range will be processed

    // TODO: invert attributes values if possible
    // Example:
    // <rect fill='#ff0000'/>
    // <rect fill='#ff0000'/>
    // <rect/>
    // <rect fill='#ff0000'/>
    // <rect fill='#ff0000'/>
    //
    // to
    //
    // <g fill='#ff0000'>
    //   <rect/>
    //   <rect/>
    //   <rect fill='none'/>
    //   <rect/>
    //   <rect/>
    // </g>
}

fn move_nodes(attributes: &[Attribute], g_node: &mut Node, node_list: &mut [Node], range: Range<usize>) {
    let attr_ids: Vec<AId> = attributes.iter().map(|a| a.id().unwrap()).collect();

    for node in node_list.iter_mut().skip(range.start).take(range.end - range.start + 1) {
        // Remove attributes from nodes.
        node.remove_attributes(&attr_ids);
        // Move them to the 'g' element.
        node.detach();
        g_node.append(node);
    }

    // Set moved attributes to the 'g' element.
    for attr in attributes {
        if attr.id().unwrap() == AId::Transform && g_node.has_attribute(AId::Transform) {
            if let AttributeValue::Transform(ref child_ts) = attr.value {
                let mut attrs = g_node.attributes_mut();
                let av = attrs.get_value_mut(AId::Transform);
                if let Some(&mut AttributeValue::Transform(ref mut ts)) = av {
                    ts.append(child_ts);
                }
            }
        } else {
            g_node.set_attribute(attr.clone());
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, ToStringWithOptions, WriteOptions};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_str($in_text).unwrap();
                group_by_style(&doc, &WriteOptions::default());
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    // Group elements with equal style.
    test!(group_1,
"<svg>
    <rect id='r1' fill='#ff0000'/>
    <rect id='r2' fill='#ff0000'/>
    <rect id='r3' fill='#ff0000'/>
</svg>",
"<svg fill='#ff0000'>
    <rect id='r1'/>
    <rect id='r2'/>
    <rect id='r3'/>
</svg>
");

    // Group elements with equal style to an existing group.
    test!(group_2,
"<svg>
    <g>
        <rect id='r1' fill='#ff0000'/>
        <rect id='r2' fill='#ff0000'/>
        <rect id='r3' fill='#ff0000'/>
    </g>
</svg>",
"<svg>
    <g fill='#ff0000'>
        <rect id='r1'/>
        <rect id='r2'/>
        <rect id='r3'/>
    </g>
</svg>
");

    // Mixed order.
    test!(group_3,
"<svg>
    <rect id='r1' fill='#ff0000'/>
    <rect id='r2'/>
    <rect id='r3' fill='#ff0000'/>
    <rect id='r4' fill='#ff0000'/>
    <rect id='r5' fill='#ff0000'/>
</svg>",
"<svg>
    <rect id='r1' fill='#ff0000'/>
    <rect id='r2'/>
    <g fill='#ff0000'>
        <rect id='r3'/>
        <rect id='r4'/>
        <rect id='r5'/>
    </g>
</svg>
");

    // Find most popular.
    test!(group_4,
"<svg>
    <rect id='r1' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r2' stroke='#00ff00'/>
    <rect id='r3' fill='#ff0000' stroke='#00ff00'/>
</svg>",
"<svg stroke='#00ff00'>
    <rect id='r1' fill='#ff0000'/>
    <rect id='r2'/>
    <rect id='r3' fill='#ff0000'/>
</svg>
");

    // Do not group 'defs'.
    test!(group_5,
"<svg>
    <rect id='r1' fill='#ff0000'/>
    <defs/>
    <rect id='r2' fill='#ff0000'/>
    <rect id='r3' fill='#ff0000'/>
    <rect id='r4' fill='#ff0000'/>
</svg>",
"<svg>
    <rect id='r1' fill='#ff0000'/>
    <defs/>
    <g fill='#ff0000'>
        <rect id='r2'/>
        <rect id='r3'/>
        <rect id='r4'/>
    </g>
</svg>
");

    // Do not group 'defs'.
    // Elements must be grouped into a new group.
    test!(group_5_1,
"<svg>
    <g>
        <rect id='r1' fill='#ff0000'/>
        <defs/>
        <rect id='r2' fill='#ff0000'/>
        <rect id='r3' fill='#ff0000'/>
        <rect id='r4' fill='#ff0000'/>
    </g>
</svg>",
"<svg>
    <g>
        <rect id='r1' fill='#ff0000'/>
        <defs/>
        <g fill='#ff0000'>
            <rect id='r2'/>
            <rect id='r3'/>
            <rect id='r4'/>
        </g>
    </g>
</svg>
");

    // Test IRI.
    test!(group_6,
"<svg>
    <linearGradient id='lg1'/>
    <rect id='r1' fill='url(#lg1)'/>
    <rect id='r2' fill='url(#lg1)'/>
    <rect id='r3' fill='url(#lg1)'/>
</svg>",
"<svg>
    <linearGradient id='lg1'/>
    <g fill='url(#lg1)'>
        <rect id='r1'/>
        <rect id='r2'/>
        <rect id='r3'/>
    </g>
</svg>
");

    // Complex order.
    test!(group_7,
"<svg>
    <rect id='r1' stroke='#00ff00'/>
    <rect id='r2' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r3' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r4' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r5' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r6' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r7' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r8' stroke='#00ff00'/>
</svg>",
"<svg stroke='#00ff00'>
    <rect id='r1'/>
    <g fill='#ff0000'>
        <rect id='r2'/>
        <rect id='r3'/>
        <rect id='r4'/>
        <rect id='r5'/>
        <rect id='r6'/>
        <rect id='r7'/>
    </g>
    <rect id='r8'/>
</svg>
");

//     // Complex order.
//     test!(group_8,
// "<svg>
//     <rect id='r1' fill='#ff0000'/>
//     <rect id='r2' fill='#ff0000'/>
//     <rect id='r3' fill='#ff0000'/>
//     <rect id='r4'/>
//     <rect id='r5' fill='#ff0000'/>
//     <rect id='r6' fill='#ff0000'/>
//     <rect id='r7' fill='#ff0000'/>
// </svg>",
// "<svg>
//     <g fill='#ff0000'>
//         <rect id='r1'/>
//         <rect id='r2'/>
//         <rect id='r3'/>
//     </g>
//     <rect id='r4'/>
//     <g fill='#ff0000'>
//         <rect id='r5'/>
//         <rect id='r6'/>
//         <rect id='r7'/>
//     </g>
// </svg>
// ");

    // Two attributes.
    test!(group_9,
"<svg>
    <rect id='r1' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r2' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r3' fill='#ff0000' stroke='#00ff00'/>
</svg>",
"<svg fill='#ff0000' stroke='#00ff00'>
    <rect id='r1'/>
    <rect id='r2'/>
    <rect id='r3'/>
</svg>
");

    // Choose longest.
    test!(group_10,
"<svg>
    <rect id='r1' fill='#ff0000'/>
    <rect id='r2' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r3' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r4' stroke='#00ff00'/>
    <rect id='r5' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r6' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r7' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r8' stroke='#00ff00'/>
    <rect id='r9' fill='#ff0000'/>
    <rect id='r10' fill='#ff0000'/>
</svg>",
"<svg>
    <rect id='r1' fill='#ff0000'/>
    <g stroke='#00ff00'>
        <rect id='r2' fill='#ff0000'/>
        <rect id='r3' fill='#ff0000'/>
        <rect id='r4'/>
        <rect id='r5' fill='#ff0000'/>
        <rect id='r6' fill='#ff0000'/>
        <rect id='r7' fill='#ff0000'/>
        <rect id='r8'/>
    </g>
    <rect id='r9' fill='#ff0000'/>
    <rect id='r10' fill='#ff0000'/>
</svg>
");

    // Choose longest range.
    test!(group_11,
"<svg>
    <rect id='r1' fill='#ff0000'/>
    <rect id='r2' fill='#ff0000'/>
    <rect id='r3' fill='#ff0000'/>
    <rect id='r4'/>
    <rect id='r5' fill='#ff0000'/>
    <rect id='r6' fill='#ff0000'/>
    <rect id='r7' fill='#ff0000'/>
    <rect id='r8' fill='#ff0000'/>
</svg>",
"<svg>
    <rect id='r1' fill='#ff0000'/>
    <rect id='r2' fill='#ff0000'/>
    <rect id='r3' fill='#ff0000'/>
    <rect id='r4'/>
    <g fill='#ff0000'>
        <rect id='r5'/>
        <rect id='r6'/>
        <rect id='r7'/>
        <rect id='r8'/>
    </g>
</svg>
");

    // Test range detection.
    test!(group_12,
"<svg>
    <rect id='r1'/>
    <rect id='r2'/>
    <rect id='r3' fill='#ff0000' stroke='#ff0000'/>
    <rect id='r4' fill='#ff0000' stroke='#ff0000'/>
    <rect id='r5' fill='#ff0000' stroke='#ff0000'/>
    <rect id='r6'/>
    <rect id='r7'/>
</svg>",
"<svg>
    <rect id='r1'/>
    <rect id='r2'/>
    <g fill='#ff0000' stroke='#ff0000'>
        <rect id='r3'/>
        <rect id='r4'/>
        <rect id='r5'/>
    </g>
    <rect id='r6'/>
    <rect id='r7'/>
</svg>
");

    // Do not group used elements.
    test!(group_13,
"<svg>
    <rect id='r1' fill='#ff0000'/>
    <use xlink:href='#r1'/>
    <rect id='r2' fill='#ff0000'/>
    <rect id='r3' fill='#ff0000'/>
    <rect id='r4' fill='#ff0000'/>
</svg>",
"<svg>
    <rect id='r1' fill='#ff0000'/>
    <use xlink:href='#r1'/>
    <g fill='#ff0000'>
        <rect id='r2'/>
        <rect id='r3'/>
        <rect id='r4'/>
    </g>
</svg>
");

    // Group transform too.
    test!(group_14,
"<svg>
    <rect id='r1' transform='scale(10)'/>
    <rect id='r2' transform='scale(10)'/>
    <rect id='r3' transform='scale(10)'/>
</svg>",
"<svg transform='scale(10)'>
    <rect id='r1'/>
    <rect id='r2'/>
    <rect id='r3'/>
</svg>
");

    // Group and merge transform too.
    test!(group_15,
"<svg>
    <g transform='translate(10)'>
        <rect id='r1' transform='scale(10)'/>
        <rect id='r2' transform='scale(10)'/>
        <rect id='r3' transform='scale(10)'/>
    </g>
</svg>",
"<svg>
    <g transform='matrix(10 0 0 10 10 0)'>
        <rect id='r1'/>
        <rect id='r2'/>
        <rect id='r3'/>
    </g>
</svg>
");

    // Group into existing group.
    test!(group_16,
"<svg>
    <g>
        <rect fill='#ff0000'/>
        <rect fill='#ff0000'/>
    </g>
</svg>",
"<svg>
    <g fill='#ff0000'>
        <rect/>
        <rect/>
    </g>
</svg>
");

}
