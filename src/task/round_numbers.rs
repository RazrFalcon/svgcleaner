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
    Document,
    AttributeValue,
};
use svgdom::types::path;

use task::short::AId;
use options::CleaningOptions;

pub fn round_numbers(doc: &Document, opt: &CleaningOptions) {
    let coord_precision = opt.coordinates_precision as usize;
    let prop_precision  = opt.properties_precision as usize;
    let paths_precision = opt.paths_coordinates_precision as usize;
    let ts_precision    = opt.transforms_precision as usize;

    for (_, mut node) in doc.descendants().svg() {
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
                AId::Width | AId::Height |
                AId::StrokeDasharray => {
                    match attr.value {
                        AttributeValue::Length(ref mut v) => {
                            round_number(&mut v.num, coord_precision);
                        }
                        AttributeValue::LengthList(ref mut list) => {
                            for n in list.iter_mut() {
                                round_number(&mut n.num, coord_precision);
                            }
                        }
                        _ => {}
                    }
                }

                AId::StrokeDashoffset |
                AId::StrokeMiterlimit |
                AId::StrokeWidth |
                AId::Opacity |
                AId::FillOpacity |
                AId::FloodOpacity |
                AId::StrokeOpacity |
                AId::StopOpacity |
                AId::FontSize => {
                    match attr.value {
                        AttributeValue::Number(ref mut num) => {
                            round_number(num, prop_precision);
                        }
                        AttributeValue::Length(ref mut v) => {
                            round_number(&mut v.num, prop_precision);
                        }
                        _ => {}
                    }
                }

                AId::Transform |
                AId::GradientTransform |
                AId::PatternTransform => {
                    if let AttributeValue::Transform(ref mut ts) = attr.value {
                        round_number(&mut ts.a, ts_precision);
                        round_number(&mut ts.b, ts_precision);
                        round_number(&mut ts.c, ts_precision);
                        round_number(&mut ts.d, ts_precision);
                        round_number(&mut ts.e, coord_precision);
                        round_number(&mut ts.f, coord_precision);
                    }
                }

                AId::D => {
                    if let AttributeValue::Path(ref mut p) = attr.value {
                        round_path(p, paths_precision);
                    }
                }

                AId::ViewBox |
                AId::Points => {
                    if let AttributeValue::NumberList(ref mut list) = attr.value {
                        for n in list.iter_mut() {
                            round_number(n, paths_precision);
                        }
                    }
                }

                _ => {}
            }
        }
    }
}

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

fn round_number(n: &mut f64, precision: usize) {
    *n = (*n * POW_VEC[precision]).round() / POW_VEC[precision];
}

fn round_path(path: &mut path::Path, precision: usize) {
    use svgdom::types::path::SegmentData;

    for seg in &mut path.d {
        match *seg.data_mut() {
              SegmentData::MoveTo { ref mut x, ref mut y }
            | SegmentData::LineTo { ref mut x, ref mut y }
            | SegmentData::SmoothQuadratic { ref mut x, ref mut y } => {
                round_number(x, precision);
                round_number(y, precision);
            }

            SegmentData::HorizontalLineTo { ref mut x } => {
                round_number(x, precision);
            }

            SegmentData::VerticalLineTo { ref mut y } => {
                round_number(y, precision);
            }

            SegmentData::CurveTo { ref mut x1, ref mut y1, ref mut x2, ref mut y2,
                                   ref mut x, ref mut y } => {
                round_number(x1, precision);
                round_number(y1, precision);
                round_number(x2, precision);
                round_number(y2, precision);
                round_number(x, precision);
                round_number(y, precision);
            }

            SegmentData::SmoothCurveTo { ref mut x2, ref mut y2, ref mut x, ref mut y } => {
                round_number(x2, precision);
                round_number(y2, precision);
                round_number(x, precision);
                round_number(y, precision);
            }

            SegmentData::Quadratic { ref mut x1, ref mut y1, ref mut x, ref mut y } => {
                round_number(x1, precision);
                round_number(y1, precision);
                round_number(x, precision);
                round_number(y, precision);
            }

            SegmentData::EllipticalArc { ref mut rx, ref mut ry, ref mut x_axis_rotation,
                                         ref mut x, ref mut y, .. } => {
                round_number(rx, precision);
                round_number(ry, precision);
                round_number(x_axis_rotation, precision);
                round_number(x, precision);
                round_number(y, precision);
            }

            SegmentData::ClosePath => {}
        }
    }
}
