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

use svgdom::{Document, Node, Attribute};

struct AttrCount {
    attr: Attribute,
    count: i32,
}

pub fn move_styles_to_group(doc: &Document) {
    let mut attrs_list: Vec<AttrCount> = Vec::new();

    _move_styles_to_group(&doc.svg_element().unwrap(), &mut attrs_list);
}

fn _move_styles_to_group(parent: &Node, attrs_list: &mut Vec<AttrCount>) {
    for node in parent.children() {
        if node.is_tag_id(EId::G) {
            _move_styles_to_group(&node, attrs_list);
            process_g(&node, attrs_list);
        }
    }
}

fn process_g(g: &Node, attrs_list: &mut Vec<AttrCount>) {
    attrs_list.clear();

    // We can't move styles from the element if it used,
    // because `use` element processes only direct styles.
    if g.children().any(|n| n.is_used()) {
        return;
    }

    for node in g.children() {
        let attrs = node.attributes();
        for attr in attrs.iter() {
            if attr.is_inheritable() && !attr.is_func_link() && attr.visible {
                let idx = attrs_list.iter().position(|a| a.attr == *attr);
                match idx {
                    Some(i) => {
                        attrs_list[i].count += 1;
                    }
                    None => {
                        // attr copy is cheap, because presentation attributes
                        // does not have any big values
                        attrs_list.push(AttrCount{ attr: attr.clone(), count: 1 });
                    }
                }
            }
        }
    }

    let children_count = g.children().count() as i32;

    for counter in attrs_list.iter() {
        if counter.count == children_count {
            g.set_attribute(counter.attr.id, counter.attr.value.clone());

            for node in g.children() {
                node.remove_attribute(counter.attr.id);
            }
        }
    }

    // TODO: 'transform'
    // TODO: partial move
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, WriteToString};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, move_styles_to_group, $in_text, $out_text);
        )
    }

    macro_rules! test_eq {
        ($name:ident, $in_text:expr) => (
            test!($name, $in_text, String::from_utf8_lossy($in_text));
        )
    }

    test!(move_1,
b"<svg>
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

    test_eq!(keep_1,
b"<svg>
    <g>
        <rect opacity='0.5'/>
        <rect opacity='0.5'/>
    </g>
</svg>
");

    test_eq!(keep_2,
b"<svg>
    <g>
        <rect fill='#ff0000'/>
        <rect/>
    </g>
</svg>
");

    test_eq!(keep_3,
b"<svg>
    <g>
        <rect id='r1' fill='#ff0000'/>
        <rect id='r2' fill='#ff0000'/>
    </g>
    <use xlink:href='#r1'/>
</svg>
");

}
