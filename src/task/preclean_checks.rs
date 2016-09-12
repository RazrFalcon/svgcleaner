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

use svgdom::{Document, TagName};

use error::CleanerError;

pub fn preclean_checks(doc: &Document) -> Result<(), CleanerError> {
    try!(check_for_unsupported_elements(doc));
    try!(check_for_script_attributes(doc));
    try!(check_for_conditional_attributes(doc));

    Ok(())
}

fn check_for_unsupported_elements(doc: &Document) -> Result<(), CleanerError> {
    for node in doc.descendants() {
        match node.tag_name() {
            Some(v) => {
                match *v {
                    TagName::Id(ref id) => {
                        match *id {
                            EId::Script => {
                                return Err(CleanerError::ScriptingIsNotSupported);
                            }
                            EId::Animate |
                            EId::Set |
                            EId::AnimateMotion |
                            EId::AnimateColor |
                            EId::AnimateTransform => {
                                return Err(CleanerError::AnimationIsNotSupported);
                            }
                            _ => {}
                        }
                    }
                    TagName::Name(_) => {},
                }
            }
            None => {}
        }
    }

    Ok(())
}

static SCRIPT_ATTRIBUTES: &'static [AId] = &[
    AId::Onabort,
    AId::Onactivate,
    AId::Onbegin,
    AId::Onclick,
    AId::Onend,
    AId::Onerror,
    AId::Onfocusin,
    AId::Onfocusout,
    AId::Onload,
    AId::Onmousedown,
    AId::Onmousemove,
    AId::Onmouseout,
    AId::Onmouseover,
    AId::Onmouseup,
    AId::Onrepeat,
    AId::Onresize,
    AId::Onscroll,
    AId::Onunload,
    AId::Onzoom,
    AId::ContentScriptType,
];

fn check_for_script_attributes(doc: &Document) -> Result<(), CleanerError> {
    for node in doc.descendants() {
        match node.tag_name() {
            Some(v) => {
                match *v {
                    TagName::Id(_) => {
                        for a in SCRIPT_ATTRIBUTES {
                            if node.has_attribute(*a) {
                                return Err(CleanerError::ScriptingIsNotSupported);
                            }
                        }
                    }
                    TagName::Name(_) => {},
                }
            }
            None => {}
        }
    }

    Ok(())
}

fn check_for_conditional_attributes(doc: &Document) -> Result<(), CleanerError> {
    // TODO: what to do with 'requiredExtensions'?

    macro_rules! check_attr {
        ($aid:expr, $node:expr) => (
            let attrs = $node.attributes();
            let s = attrs.get_value($aid).unwrap().as_string().unwrap();
            if !s.is_empty() {
                return Err(CleanerError::ConditionalProcessingIsNotSupported);
            }
        )
    }

    for node in doc.descendants() {
        if node.has_attribute(AId::RequiredFeatures) {
            check_attr!(AId::RequiredFeatures, node);
        } else if node.has_attribute(AId::SystemLanguage) {
            check_attr!(AId::SystemLanguage, node);
        }
    }

    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::Document;
    use error::CleanerError;

    #[test]
    fn test_scripting_1() {
        let doc = Document::from_data(
b"<svg>
    <script/>
</svg>
").unwrap();

        assert_eq!(preclean_checks(&doc).err().unwrap(), CleanerError::ScriptingIsNotSupported);
    }

    #[test]
    fn test_scripting_2() {
        let doc = Document::from_data(
b"<svg onload=''/>").unwrap();
        assert_eq!(preclean_checks(&doc).err().unwrap(), CleanerError::ScriptingIsNotSupported);
    }

    #[test]
    fn test_animation_2() {
        let doc = Document::from_data(
b"<svg>
    <set/>
</svg>
").unwrap();

        assert_eq!(preclean_checks(&doc).err().unwrap(), CleanerError::AnimationIsNotSupported);
    }
}
