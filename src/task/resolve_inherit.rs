// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

use svgdom::{
    AttributeId,
    AttributeValue,
    Document,
    Node,
    ValueId,
};

use error::{
    ErrorKind,
    Result,
};

// TODO: split
/// Resolve `inherit` and `currentColor` attributes.
pub fn resolve_inherit(doc: &Document) -> Result<()> {
    let mut vec_inherit = Vec::new();
    let mut vec_curr_color = Vec::new();

    for (_, mut node) in doc.descendants().svg() {
        vec_inherit.clear();
        vec_curr_color.clear();

        {
            let attrs = node.attributes();
            for (aid, attr) in attrs.iter_svg() {
                if let AttributeValue::PredefValue(ref v) = attr.value {
                    match *v {
                        ValueId::Inherit => {
                            vec_inherit.push(aid);
                        }
                        ValueId::CurrentColor => {
                            vec_curr_color.push(aid);
                        }
                        _ => {},
                    }
                }
            }
        }

        for id in &vec_inherit {
            resolve_impl(&mut node, *id, *id)?;
        }

        for id in &vec_curr_color {
            let av = node.attributes().get_value(AttributeId::Color).cloned();
            if let Some(av) = av {
                node.set_attribute((*id, av.clone()));
            } else {
                resolve_impl(&mut node, *id, AttributeId::Color)?;
            }
        }
    }

    Ok(())
}

fn resolve_impl(node: &mut Node, curr_attr: AttributeId, parent_attr: AttributeId) -> Result<()> {
    if let Some(n) = node.parents().find(|n| n.has_attribute(parent_attr)) {
        let av = n.attributes().get_value(parent_attr).cloned();
        if let Some(av) = av {
            node.set_attribute((curr_attr, av.clone()));
        }

        Ok(())
    } else {
        Err(ErrorKind::UnresolvedAttribute(curr_attr.to_string()).into())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, ToStringWithOptions};

    #[cfg(test)]
    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let mut doc = Document::from_str($in_text).unwrap();
                resolve_inherit(&mut doc).unwrap();
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    #[cfg(test)]
    macro_rules! test_err {
        ($name:ident, $in_text:expr) => (
            #[test]
            fn $name() {
                let mut doc = Document::from_str($in_text).unwrap();
                assert_eq!(resolve_inherit(&mut doc).is_err(), true);
            }
        )
    }

    test!(inherit_1,
"<svg fill='#ff0000'>
    <rect fill='inherit'/>
</svg>",
"<svg fill='#ff0000'>
    <rect fill='#ff0000'/>
</svg>
");

    test!(inherit_2,
"<svg fill='#ff0000'>
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
"<svg fill='#ff0000' stroke='#00ff00'>
    <rect fill='inherit' stroke='inherit'/>
</svg>",
"<svg fill='#ff0000' stroke='#00ff00'>
    <rect fill='#ff0000' stroke='#00ff00'/>
</svg>
");

    test!(current_color_1,
"<svg color='#ff0000'>
    <rect fill='currentColor'/>
</svg>",
"<svg color='#ff0000'>
    <rect fill='#ff0000'/>
</svg>
");

    test!(current_color_2,
"<svg>
    <rect color='#ff0000' fill='currentColor'/>
</svg>",
"<svg>
    <rect color='#ff0000' fill='#ff0000'/>
</svg>
");

    test!(current_color_3,
"<svg color='#ff0000'>
    <rect fill='currentColor' stroke='currentColor'/>
</svg>",
"<svg color='#ff0000'>
    <rect fill='#ff0000' stroke='#ff0000'/>
</svg>
");

    test_err!(unresolvable_1,
"<svg>
    <rect fill='inherit'/>
</svg>"
);

    test_err!(unresolvable_2,
"<svg>
    <rect fill='currentColor'/>
    <rect fill='inherit'/>
</svg>"
);

    test_err!(unresolvable_3,
"<svg font-family='inherit'/>"
);
}
