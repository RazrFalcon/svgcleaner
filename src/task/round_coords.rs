/****************************************************************************
**
** svgcleaner could help you to clean up your SVG files
** from unnecessary data.
** Copyright (C) 2012-2017 Evgeniy Reizner
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

use super::short::AId;

use svgdom::{Document, AttributeValue};

use options::Options;

static POW_VEC: &'static [f64] = &[
                    0.0,
                   10.0,
                  100.0,
                1_000.0,
               10_000.0,
              100_000.0,
            1_000_000.0,
           10_000_000.0,
          100_000_000.0,
        1_000_000_000.0,
       10_000_000_000.0,
      100_000_000_000.0,
    1_000_000_000_000.0,
];

pub fn round_coordinates(doc: &Document, options: &Options) {
    let coord_precision = options.coordinates_precision as usize;
    let ts_precision = options.transform_precision as usize;

    for node in doc.descendants().svg() {
        let mut attrs = node.attributes_mut();

        for (aid, ref mut attr) in attrs.iter_svg_mut() {
            match aid {
                AId::X  | AId::Y |
                AId::Dx | AId::Dy |
                AId::X1 | AId::Y1 |
                AId::X2 | AId::Y2 |
                AId::R  |
                AId::Rx | AId::Ry |
                AId::Cx | AId::Cy |
                AId::Fx | AId::Fy |
                AId::Width | AId::Height => {
                    match attr.value {
                        AttributeValue::Length(ref mut v) => {
                            round_number(&mut v.num, coord_precision);
                        }
                        _ => {}
                    }
                }
                AId::Transform |
                AId::GradientTransform |
                AId::PatternTransform => {
                    match attr.value {
                        AttributeValue::Transform(ref mut ts) => {
                            round_number(&mut ts.a, ts_precision);
                            round_number(&mut ts.b, ts_precision);
                            round_number(&mut ts.c, ts_precision);
                            round_number(&mut ts.d, ts_precision);
                            round_number(&mut ts.e, coord_precision);
                            round_number(&mut ts.f, coord_precision);
                        }
                        _ => {}
                    }
                }
                _ => {}
            }
        }
    }
}

fn round_number(n: &mut f64, precision: usize) {
    *n = (*n * POW_VEC[precision]).round() / POW_VEC[precision];
}
