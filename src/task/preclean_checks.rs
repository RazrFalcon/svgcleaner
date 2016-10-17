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

use svgdom::{Document, AttributeValue};

use error::CleanerError;

pub fn preclean_checks(doc: &Document) -> Result<(), CleanerError> {
    try!(check_for_unsupported_elements(doc));
    try!(check_for_script_attributes(doc));
    try!(check_for_conditional_attributes(doc));
    try!(check_for_external_xlink(doc));

    Ok(())
}

fn check_for_unsupported_elements(doc: &Document) -> Result<(), CleanerError> {
    for node in doc.descendants().svg() {
        match node.tag_id().unwrap() {
            EId::Script => {
                return Err(CleanerError::ScriptingIsNotSupported);
            }
              EId::Animate
            | EId::Set
            | EId::AnimateMotion
            | EId::AnimateColor
            | EId::AnimateTransform => {
                return Err(CleanerError::AnimationIsNotSupported);
            }
            _ => {}
        }
    }

    Ok(())
}

fn check_for_script_attributes(doc: &Document) -> Result<(), CleanerError> {
    for node in doc.descendants().svg() {
        let attrs = node.attributes();
        for attr in attrs.iter() {
            if attr.is_graphical_event() || attr.is_document_event() || attr.is_animation_event() {
                return Err(CleanerError::ScriptingIsNotSupported);
            }
        }
    }

    Ok(())
}

fn check_for_conditional_attributes(doc: &Document) -> Result<(), CleanerError> {
    // TODO: what to do with 'requiredExtensions'?

    macro_rules! check_attr {
        ($aid:expr, $node:expr) => (
            let attrs = $node.attributes();
            if let Some(av) = attrs.get_value($aid) {
                // libsvgdom doesn't parse this attributes, so they must have String type
                let s = av.as_string().unwrap();
                if !s.is_empty() {
                    // NOTE: we are only care about non-empty attributes
                    return Err(CleanerError::ConditionalProcessingIsNotSupported);
                }
            }
        )
    }

    for node in doc.descendants().svg() {
        check_attr!(AId::RequiredFeatures, node);
        check_attr!(AId::SystemLanguage, node);
    }

    Ok(())
}

fn check_for_external_xlink(doc: &Document) -> Result<(), CleanerError> {
    for node in doc.descendants().svg() {
        if !node.has_attribute(AId::XlinkHref) {
            continue;
        }

        match node.tag_id().unwrap() {
              EId::A
            | EId::Image
            | EId::FontFaceUri
            | EId::FeImage => continue,
            _ => {}
        }

        let attrs = node.attributes();
        if let AttributeValue::String(ref s) = *attrs.get_value(AId::XlinkHref).unwrap() {
            return Err(CleanerError::ExternalHrefIsNotSupported(s.clone()));
        }
    }

    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::Document;
    use error::CleanerError;

    macro_rules! test {
        ($name:ident, $in_text:expr, $err:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_data($in_text).unwrap();
                assert_eq!(preclean_checks(&doc).err().unwrap(), $err);
            }
        )
    }

    macro_rules! test_ok {
        ($name:ident, $in_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_data($in_text).unwrap();
                assert_eq!(preclean_checks(&doc).is_ok(), true);
            }
        )
    }

    test!(test_scripting_1, b"<svg><script/></svg>",
          CleanerError::ScriptingIsNotSupported);

    test!(test_scripting_2, b"<svg onload=''/>",
          CleanerError::ScriptingIsNotSupported);

    test!(test_animation_1, b"<svg><set/></svg>",
          CleanerError::AnimationIsNotSupported);

    test!(test_conditions_1, b"<svg><switch requiredFeatures='text'/></svg>",
          CleanerError::ConditionalProcessingIsNotSupported);

    test!(test_conditions_2, b"<svg><switch systemLanguage='en'/></svg>",
          CleanerError::ConditionalProcessingIsNotSupported);

    test_ok!(test_conditions_3, b"<svg><switch requiredFeatures=''/></svg>");
    test_ok!(test_conditions_4, b"<svg><switch systemLanguage=''/></svg>");
}
