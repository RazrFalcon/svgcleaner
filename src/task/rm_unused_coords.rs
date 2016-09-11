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

pub fn remove_unused_coordinates(doc: &Document) {
    let mut rm_list = Vec::with_capacity(16);

    for node in doc.descendants() {
        {
            let attrs = node.attributes();

            // TODO: x, y, dx, dy of the 'tspan' element
            match node.tag_id().unwrap() {
                EId::Svg => {
                    // 'x' and 'y' attributes of the root 'svg' element are pointless.
                    // https://www.w3.org/TR/SVG/struct.html#SVGElementXAttribute
                    if !node.has_parent() {
                        if attrs.contains(AId::X) {
                            rm_list.push(AId::X);
                        }
                        if attrs.contains(AId::Y) {
                            rm_list.push(AId::Y);
                        }
                    }
                }
                EId::Rect => {
                    // if 'rx' equals to 'ry', than we can remove 'ry'
                    // https://www.w3.org/TR/SVG/shapes.html#RectElementRYAttribute
                    if attrs.contains(AId::Rx) && attrs.contains(AId::Ry) {
                        if attrs.get_value(AId::Rx).unwrap() == attrs.get_value(AId::Ry).unwrap() {
                            rm_list.push(AId::Ry);
                        }
                    }
                }
                EId::RadialGradient => {
                    // https://www.w3.org/TR/SVG/pservers.html#RadialGradientElementFXAttribute

                    // we don't check parent attributes, since we already resolved them
                    // in resolve_attrs::radial_gradients

                    if attrs.contains(AId::XlinkHref) {
                        continue;
                    }

                    macro_rules! rm_f {
                        ($f:expr, $c:expr) => (
                            if attrs.contains($f) && attrs.contains($c) {
                                if attrs.get_value($f).unwrap() == attrs.get_value($c).unwrap() {
                                    // we can remove 'fx'/'fy' only if this element is not used
                                    // by any radialGradient
                                    let c = node.linked_nodes()
                                                .filter(|n| n.is_tag_id(EId::RadialGradient))
                                                .count();

                                    if c == 0 {
                                        rm_list.push($f);
                                    }
                                }
                            })
                    }

                    // if 'fx' equals to 'cx', than we can remove 'fx'
                    rm_f!(AId::Fx, AId::Cx);
                    // if 'fy' equals to 'cy', than we can remove 'fy'
                    rm_f!(AId::Fy, AId::Cy);
                }
                _ => {}
            }
        }

        {
            let mut attrs_mut = node.attributes_mut();
            for aid in &rm_list {
                // we do not remove unused attributes, just hiding them
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
            base_test!($name, remove_unused_coordinates, $in_text, $out_text);
        )
    }

    macro_rules! test_eq {
        ($name:ident, $in_text:expr) => (
            test!($name, $in_text, String::from_utf8_lossy($in_text));
        )
    }

    test!(rm_svg_x_y,
b"<svg x='5' y='5'>
    <svg x='5' y='5'/>
</svg>",
"<svg>
    <svg x='5' y='5'/>
</svg>
");

    test!(rm_rect_rx_ry,
b"<svg>
    <rect rx='5' ry='5'/>
    <rect rx='5' ry='5em'/>
</svg>",
"<svg>
    <rect rx='5'/>
    <rect rx='5' ry='5em'/>
</svg>
");

    test!(rm_radial_gradient_fx,
b"<svg>
    <radialGradient cx='5' cy='5' fx='5' fy='5'/>
</svg>",
"<svg>
    <radialGradient cx='5' cy='5'/>
</svg>
");

    test_eq!(keep_radial_gradient_fx_1,
b"<svg>
    <radialGradient id='rg1' fx='5'/>
    <radialGradient xlink:href='#rg1'/>
</svg>
");

    test_eq!(keep_radial_gradient_fx_2,
b"<svg>
    <radialGradient id='rg1'/>
    <radialGradient fx='5' xlink:href='#rg1'/>
</svg>
");
}
