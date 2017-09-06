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
    AttributeType,
    Document,
    WriteBuffer,
    WriteOptions,
};

use task::short::AId;
use options::StyleJoinMode;

pub fn join_style_attributes(doc: &Document, mode: StyleJoinMode, opt: &WriteOptions) {
    // NOTE: Must be run at last, since it breaks linking.

    if mode == StyleJoinMode::None {
        return;
    }

    for (_, mut node) in doc.descendants().svg() {
        let count = {
            let attrs = node.attributes();
            attrs.iter().filter(|a| a.is_presentation() && a.visible).count()
        };

        // 5 - is an amount of attributes when style notation is becoming more efficient than
        // split attributes.
        if (mode == StyleJoinMode::Some && count > 5) || mode == StyleJoinMode::All {
            let mut attrs = node.attributes_mut();
            let mut ids = Vec::new();
            let mut style = Vec::new();
            for (aid, attr) in attrs.iter_svg().filter(|&(_, a)| a.is_presentation()) {
                if !attr.visible {
                    continue;
                }

                style.extend_from_slice(aid.name().as_bytes());
                style.push(b':');
                attr.value.write_buf_opt(opt, &mut style);
                style.push(b';');

                ids.push(aid);
            }
            style.pop();

            // Unwrap can't fail.
            let style_str = String::from_utf8(style).unwrap();
            attrs.insert_from(AId::Style, style_str);

            for id in ids {
                // Use "private" method, because we breaking linking on purpose.
                // Default 'remove()' will panic here.
                attrs.remove_impl(id);
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use task::short::AId;
    use svgdom::{Document, WriteOptions, AttributeValue};
    use options::StyleJoinMode;

    #[test]
    fn join_styles_1() {
        let doc = Document::from_str(
            "<svg fill='black' stroke='red' stroke-width='1' opacity='1' \
                  fill-opacity='1' stroke-opacity='1'/>"
        ).unwrap();

        let svg_node = doc.svg_element().unwrap();
        let wopt = WriteOptions::default();

        join_style_attributes(&doc, StyleJoinMode::None, &wopt);
        assert_eq!(svg_node.attributes().get(AId::Style), None);

        // We have 6 style attributes so they should be joined.
        join_style_attributes(&doc, StyleJoinMode::Some, &wopt);

        let res = match svg_node.attributes().get_value(AId::Style).cloned() {
            Some(AttributeValue::String(s)) => s,
            _ => unreachable!(),
        };

        assert_eq_text!(
            res,
            "fill:#000000;stroke:#ff0000;stroke-width:1;opacity:1;fill-opacity:1;stroke-opacity:1"
        );
    }

    #[test]
    fn join_styles_2() {
        let doc = Document::from_str(
            "<svg fill='black' stroke='red'/>"
        ).unwrap();

        let svg_node = doc.svg_element().unwrap();
        let wopt = WriteOptions::default();

        // We have only 2 style attributes so they shouldn't be joined.
        join_style_attributes(&doc, StyleJoinMode::Some, &wopt);
        assert_eq!(svg_node.attributes().get(AId::Style), None);

        // Join anyway.
        join_style_attributes(&doc, StyleJoinMode::All, &wopt);

        let res = match svg_node.attributes().get_value(AId::Style).cloned() {
            Some(AttributeValue::String(s)) => s,
            _ => unreachable!(),
        };

        assert_eq_text!(
            res,
            "fill:#000000;stroke:#ff0000"
        );
    }
}
