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

use std::fmt;
use std::cmp;

use super::short::{EId, AId};

use svgdom::{Document, Node, Attribute, AttributeType};

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

    /// Removes unneeded rows.
    fn simplify(&mut self) {
        // table should be already defined/filled
        debug_assert!(!self.d.is_empty());
        debug_assert!(self.d[0].flags.len() >= 3);

        // we group only three or more elements,
        // so rows with less than 3 flags are useless
        self.d.retain(|ref x| {
            x.count_flags() > 2
        });

        // rows should contain flags that repeats at least 3 times
        // |*-*| -> |---|
        // |**-**| -> |-----|
        // |*-*-*-*| -> |-------|
        // |***-*-*| -> |***----|
        // etc.

        for d in self.d.iter_mut() {
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

        // remove again
        self.d.retain(|ref x| {
            x.count_flags() > 2
        });
    }

    /// Sorts rows by a longest continuous range of set flags.
    fn sort(&mut self) {
        self.d.sort_by(|a, b| {
            let ac = a.longest();
            let bc = b.longest();
            bc.cmp(&ac)
        });
    }

    /// Joins rows.
    fn join(&mut self) {
        // replace
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
    /// Returns an amount of set/true flags.
    fn count_flags(&self) -> usize {
        self.flags.iter().filter(|f| **f).count()
    }

    /// Returns a length of longest continuous range of set flags.
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

    /// Returns a longest continuous range of set flags.
    ///
    /// # Example
    ///
    /// `|-***-*****--|` -> (5, 10)
    fn longest_range(&self) -> (usize, usize) {
        let mut list = Vec::new();

        let mut start = None;
        let mut end = 0;
        for (idx, f) in self.flags.iter().enumerate() {
            if *f == true {
                if start.is_none() {
                    start = Some(idx);
                }

                end = idx;
            } else {
                if let Some(s) = start {
                    list.push((s, end));
                    start = None;
                    end = 0;
                }
            }
        }
        if let Some(s) = start {
            list.push((s, end));
        }

        list.sort_by(|a, b| (b.1 - b.0).cmp(&(a.1 - a.0)));

        list[0]
    }
}

impl fmt::Debug for Table {
    /// Prints something like:
    /// fill="#ff0000"   |*-*|
    /// stroke="#00ff00" |***|
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
            s.push_str(&line);

            // indent
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

                table_in.simplify();

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

    // we use dummy attributes since we care only about flags

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

    // we care only about longest range, not about number of set flags
    test_all!(sort_2,
        ["*****-******", "**********--"],
        "fill=\"red\" |**********--|\n\
         fill=\"red\" |*****-******|");
}

// TODO: use 'svg' instead of 'g' when possible

pub fn group_by_style(doc: &Document) {
    _group_by_style(&doc.svg_element().unwrap());
}

fn _group_by_style(parent: &Node) {
    let mut node_list = Vec::with_capacity(16);

    // collect nodes
    // TODO: currently we ignore non-SVG elements, which is bad
    for node in parent.children().svg() {
        // If 'defs' node occurred - skip it and reset the list.

        // Node can't be used, because a 'use' retrieves only element's attributes
        // and not parent ones. So if we move attributes to the group - 'use' element
        // will be rendered incorrectly.
        if node.is_tag_name(EId::Defs) || node.is_used() {
            node_list.clear();
            continue;
        }

        // recursive processing
        if node.is_tag_name(EId::G) {
            _group_by_style(&node);
        }

        node_list.push(node);
    }

    // we should have at least 3 nodes, because there is no point in grouping one or two nodes
    if node_list.len() < 3 {
        return;
    }

    let nodes_count = node_list.len();
    let mut table = Table::new();

    // fill table with attributes
    for node in &node_list {
        let attrs = node.attributes();
        for (_, attr) in attrs.iter_svg() {
            if attr.visible && attr.is_inheritable() {
                // append only unique attributes
                if !table.d.iter().any(|ref x| x.attributes[0] == *attr) {
                    table.append(attr, nodes_count);
                }
            }
        }
    }

    if table.d.is_empty() {
        return;
    }

    // set attributes flags inside the table
    for d in table.d.iter_mut() {
        for (idx, node) in node_list.iter().enumerate() {
            let attrs = node.attributes();
            if attrs.iter().any(|ref a| **a == d.attributes[0]) {
                d.flags[idx] = true;
            }
        }
    }

    table.simplify();

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
        let g_node = if parent.is_tag_name(EId::G) {
            parent.clone()
        } else {
            let g_node = parent.document().create_element(EId::G);
            node_list[0].insert_before(g_node.clone());
            g_node
        };

        move_nodes(&table.d[0].attributes, &g_node, &node_list, (0, node_list.len()));

        // remove first row
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

        // get longest range on nodes
        let (start, end) = d.longest_range();

        // do the same as in previous block
        let g_node = parent.document().create_element(EId::G);
        node_list.iter().nth(start).unwrap().insert_before(g_node.clone());

        move_nodes(&d.attributes, &g_node, &node_list, (start, end));
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

fn move_nodes(attributes: &Vec<Attribute>, g_node: &Node, node_list: &Vec<Node>,
              range: (usize, usize)) {
    let attr_ids: Vec<AId> = attributes.iter().map(|a| a.id().unwrap()).collect();

    for node in node_list.iter().skip(range.0).take(range.1 - range.0 + 1) {
        // remove attributes from nodes
        node.remove_attributes(&attr_ids);
        // move them to the 'g' element.
        node.detach();
        g_node.append(node);
    }

    // set moved attributes to the 'g' element
    for attr in attributes {
        g_node.set_attribute_object(attr.clone());
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, WriteToString};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_data($in_text).unwrap();
                group_by_style(&doc);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    // group elements with equal style
    test!(group_1,
b"<svg>
    <rect id='r1' fill='#ff0000'/>
    <rect id='r2' fill='#ff0000'/>
    <rect id='r3' fill='#ff0000'/>
</svg>",
"<svg>
    <g fill='#ff0000'>
        <rect id='r1'/>
        <rect id='r2'/>
        <rect id='r3'/>
    </g>
</svg>
");

    // group elements with equal style to an existing group
    test!(group_2,
b"<svg>
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

    // mixed order
    test!(group_3,
b"<svg>
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

    // find most popular
    test!(group_4,
b"<svg>
    <rect id='r1' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r2' stroke='#00ff00'/>
    <rect id='r3' fill='#ff0000' stroke='#00ff00'/>
</svg>",
"<svg>
    <g stroke='#00ff00'>
        <rect id='r1' fill='#ff0000'/>
        <rect id='r2'/>
        <rect id='r3' fill='#ff0000'/>
    </g>
</svg>
");

    // do not group 'defs'
    test!(group_5,
b"<svg>
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

    // test IRI
    test!(group_6,
b"<svg>
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

    // complex order
    test!(group_7,
b"<svg>
    <rect id='r1' stroke='#00ff00'/>
    <rect id='r2' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r3' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r4' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r5' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r6' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r7' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r8' stroke='#00ff00'/>
</svg>",
"<svg>
    <g stroke='#00ff00'>
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
    </g>
</svg>
");

//     // complex order
//     test!(group_8,
// b"<svg>
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

    // two attributes
    test!(group_9,
b"<svg>
    <rect id='r1' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r2' fill='#ff0000' stroke='#00ff00'/>
    <rect id='r3' fill='#ff0000' stroke='#00ff00'/>
</svg>",
"<svg>
    <g fill='#ff0000' stroke='#00ff00'>
        <rect id='r1'/>
        <rect id='r2'/>
        <rect id='r3'/>
    </g>
</svg>
");

    // choose longest
    test!(group_10,
b"<svg>
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

    // choose longest range
    test!(group_11,
b"<svg>
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

    // test range detection
    test!(group_12,
b"<svg>
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

    // do not group used elements
    test!(group_13,
b"<svg>
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

}
