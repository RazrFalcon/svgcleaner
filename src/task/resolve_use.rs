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
};
use svgdom::types::{
    Length,
    Transform,
};

use task::short::{EId, AId, Unit};

pub fn resolve_use(doc: &Document) {
    let mut nodes = Vec::new();

    for node in doc.descendants().filter(|n| n.is_tag_name(EId::Use)) {
        if let Some(value) = node.attributes().get_value(AId::XlinkHref) {
            if let AttributeValue::Link(ref link) = *value {

                // Resolve elements that linked to elements inside 'defs'.
                let parent = link.parent().unwrap();
                if !parent.is_tag_name(EId::Defs) {
                    continue;
                }

                // 'symbol' elements can't be simply replaced.
                if link.is_tag_name(EId::Symbol) {
                    continue;
                }

                // Ignore 'use' elements linked to other 'use' elements.
                if link.is_tag_name(EId::Use) {
                    continue;
                }

                // Element should be used only once.
                if link.uses_count() != 1 {
                    continue;
                }

                nodes.push((node.clone(), link.clone()));
            }
        }
    }

    for (mut node, mut link) in nodes {
        // Unlink 'use'.
        node.remove_attribute(AId::XlinkHref);

        {
            // 'use' element support 'x', 'y' and 'transform' attributes and we should process them
            // so we apply translate transform to the linked element transform.

            let mut attrs = node.attributes_mut();

            // 'x' or 'y' should be set.
            if attrs.contains(AId::X) || attrs.contains(AId::Y) {
                let x = get_value!(attrs, Length, AId::X, Length::zero());
                let y = get_value!(attrs, Length, AId::Y, Length::zero());

                // We can apply 'x' and 'y' to transform only when they have 'none' units.
                if !(x.unit == Unit::None && y.unit == Unit::None) {
                    continue;
                }

                let mut ts = get_value!(attrs, Transform, AId::Transform, Transform::default());
                ts.translate(x.num, y.num);

                attrs.insert_from(AId::Transform, ts);
                attrs.remove(AId::X);
                attrs.remove(AId::Y);
            }
        }

        // Copy attributes.
        for attr in node.attributes().iter() {
            link.set_attribute(attr.clone());
        }

        // TODO: maybe just change the tag name
        node.insert_after(&link);

        node.remove();
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, ToStringWithOptions};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, resolve_use, $in_text, $out_text);
        )
    }

    test!(resolve_1,
"<svg>
    <defs>
        <rect id='r1' width='10'/>
    </defs>
    <use xlink:href='#r1'/>
</svg>",
"<svg>
    <defs/>
    <rect id='r1' width='10'/>
</svg>
");

    test!(resolve_2,
"<svg>
    <defs>
        <rect id='r1' width='10'/>
    </defs>
    <use xlink:href='#r1' fill='#ff0000'/>
</svg>",
"<svg>
    <defs/>
    <rect id='r1' fill='#ff0000' width='10'/>
</svg>
");

    test!(resolve_3,
"<svg>
    <defs>
        <rect id='r1' width='10'/>
    </defs>
    <use x='10' xlink:href='#r1'/>
</svg>",
"<svg>
    <defs/>
    <rect id='r1' transform='translate(10)' width='10'/>
</svg>
");

    test!(resolve_4,
"<svg>
    <defs>
        <rect id='r1' width='10'/>
    </defs>
    <use x='10' xlink:href='#r1' transform='translate(10 10)'/>
</svg>",
"<svg>
    <defs/>
    <rect id='r1' transform='translate(20 10)' width='10'/>
</svg>
");

    test!(resolve_5,
"<svg>
    <defs>
        <linearGradient id='lg1'/>
        <rect id='r1' fill='url(#lg1)'/>
    </defs>
    <use xlink:href='#r1'/>
</svg>",
"<svg>
    <defs>
        <linearGradient id='lg1'/>
    </defs>
    <rect id='r1' fill='url(#lg1)'/>
</svg>
");

    test_eq!(keep_1,
"<svg>
    <rect id='r1'/>
    <use xlink:href='#r1'/>
</svg>
");

    test_eq!(keep_2,
"<svg>
    <defs>
        <symbol id='r1'>
            <rect/>
        </symbol>
    </defs>
    <use xlink:href='#r1'/>
</svg>
");
}
