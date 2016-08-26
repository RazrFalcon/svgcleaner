/****************************************************************************
**
** SVG Cleaner could help you to clean up your SVG files
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

use super::short::{AId};

use svgdom::{Document, Node, Attribute, AttributeValue, ValueId};

use error::CleanerError;

fn resolve_impl(node: &Node, curr_attr: AId, parent_attr: AId)
               -> Result<(), CleanerError> {
    match node.parent_attribute(parent_attr) {
        Some(av) => {
            node.set_attribute(curr_attr, av.value);
        }
        None => {
            match Attribute::default(curr_attr) {
                Some(a) => node.set_attribute(curr_attr, a.value),
                None => return Err(CleanerError::UnresolvedAttribute),
            }
        }
    }

    Ok(())
}

pub fn resolve_inherit(doc: &Document) -> Result<(), CleanerError> {
    for node in doc.descendants() {

        let mut vec_inherit = Vec::new();
        let mut vec_curr_color = Vec::new();

        {
            let attrs = node.attributes();
            for attr in attrs.values() {
                match &attr.value {
                    // &AttributeValue::String(ref v) => {
                    //     // TODO: remove in release
                    //     if v == "inherit" || v == "currentColor" {
                    //         println!("Warning: unparsed value: {:?}.", attr);
                    //     }
                    // }
                    &AttributeValue::PredefValue(ref v) => {
                        match v {
                            &ValueId::Inherit => {
                                vec_inherit.push(attr.id);
                            }
                            &ValueId::CurrentColor => {
                                vec_curr_color.push(attr.id);
                            }
                            _ => {},
                        }
                    }
                    _ => {},
                }
            }
        }

        for id in vec_inherit {
            try!(resolve_impl(&node, id, id));
        }

        for id in vec_curr_color {
            if node.has_attribute(AId::Color) {
                let v = node.attribute_value(AId::Color).unwrap();
                node.set_attribute(id, v.clone());
            } else {
                try!(resolve_impl(&node, id, AId::Color));
            }
        }
    }

    Ok(())
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
                resolve_inherit(&doc).unwrap();
                assert_eq_text!(doc_to_str_tests!(doc), $out_text);
            }
        )
    }

    test!(inherit_1,
b"<svg fill='#ff0000'>
    <rect fill='inherit'/>
</svg>",
"<svg fill='#ff0000'>
    <rect fill='#ff0000'/>
</svg>
");

    test!(inherit_2,
b"<svg fill='#ff0000'>
    <g>
        <rect fill='inherit'/>
    </g>
</svg>",
"<svg fill='#ff0000'>
    <g>
        <rect fill='#ff0000'/>
    </g>
</svg>
");

    test!(inherit_3,
b"<svg fill='#ff0000' stroke='#00ff00'>
    <rect fill='inherit' stroke='inherit'/>
</svg>",
"<svg fill='#ff0000' stroke='#00ff00'>
    <rect fill='#ff0000' stroke='#00ff00'/>
</svg>
");

    test!(current_color_1,
b"<svg color='#ff0000'>
    <rect fill='currentColor'/>
</svg>",
"<svg color='#ff0000'>
    <rect fill='#ff0000'/>
</svg>
");

    test!(current_color_2,
b"<svg>
    <rect color='#ff0000' fill='currentColor'/>
</svg>",
"<svg>
    <rect color='#ff0000' fill='#ff0000'/>
</svg>
");

    test!(current_color_3,
b"<svg color='#ff0000'>
    <rect fill='currentColor' stroke='currentColor'/>
</svg>",
"<svg color='#ff0000'>
    <rect fill='#ff0000' stroke='#ff0000'/>
</svg>
");

    test!(default_1,
b"<svg>
    <rect fill='currentColor'/>
    <rect fill='inherit'/>
</svg>",
"<svg>
    <rect fill='#000000'/>
    <rect fill='#000000'/>
</svg>
");
}
