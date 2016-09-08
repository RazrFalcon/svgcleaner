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

use svgdom::{Document};

pub fn remove_default_attributes(doc: &Document) {
    let mut rm_list = Vec::with_capacity(16);

    for node in doc.descendants() {
        {
            let attrs = node.attributes();

            for attr in attrs.values() {
                if attr.is_presentation() {
                    if attr.check_is_default() {
                        match node.parent_attribute(attr.id) {
                            Some(pattr) => {
                                if !pattr.visible {
                                    rm_list.push(attr.id);
                                }
                            }
                            None => {
                                rm_list.push(attr.id)
                            }
                        }
                    }
                } else {
                    // check default values of an non-presentation attributes
                    match attr.id {
                        AId::UnitsPerEm => {
                            if attr.value.as_string().unwrap() == "1000" {
                                rm_list.push(attr.id);
                            }

                        }
                        AId::Slope => {
                            // there are two different 'slope': one for 'font-face'
                            // and one for 'feFunc*'
                            let v = attr.value.as_string().unwrap();
                            if node.is_tag_id(EId::FontFace) && v == "0" {
                                rm_list.push(attr.id);
                            } else if v == "1" {
                                rm_list.push(attr.id);
                            }
                        }
                        _ => {}

                        // TODO: 'x', 'y' attributes equal to 0 is default
                        // TODO: 'width', 'height' of SVG elem is default to 100%
                    }

                    // TODO: add other
                }
            }
        }

        {
            let mut attrs_mut = node.attributes_mut();
            for aid in &rm_list {
                // we only hide default attributes, because they still can be useful
                attrs_mut.get_mut(*aid).unwrap().visible = false;
            }
        }

        rm_list.clear();
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{Document, WriteToString};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            base_test!($name, remove_default_attributes, $in_text, $out_text);
        )
    }

    test!(rm_1,
b"<svg>
    <rect fill='#000000'/>
</svg>",
"<svg>
    <rect/>
</svg>
");

    test!(keep_1,
b"<svg fill='#ff0000'>
    <rect fill='#000000'/>
</svg>",
"<svg fill='#ff0000'>
    <rect fill='#000000'/>
</svg>
");

    test!(all,
b"<svg>
    <rect fill='#000000'/>
</svg>",
"<svg>
    <rect/>
</svg>
");
}
