// svgcleaner could help you to clean up your SVG files
// from unnecessary data.
// Copyright (C) 2012-2018 Evgeniy Reizner
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

use svgdom::path::{Path, SegmentData};
use svgdom::{FuzzyEq, Transform};

pub fn apply_transform(path: &mut Path, ts: &Transform) {
    let (sx, sy) = ts.get_scale();

    // Only proportional scale is supported.
    debug_assert!(sx.fuzzy_eq(&sy));

    for seg in &mut path.d {
        match *seg.data_mut() {
            SegmentData::MoveTo {
                ref mut x,
                ref mut y,
            }
            | SegmentData::LineTo {
                ref mut x,
                ref mut y,
            }
            | SegmentData::SmoothQuadratic {
                ref mut x,
                ref mut y,
            } => {
                ts.apply_ref(x, y);
            }

            SegmentData::HorizontalLineTo { ref mut x } => {
                ts.apply_ref(x, &mut 0.0);
            }

            SegmentData::VerticalLineTo { ref mut y } => {
                ts.apply_ref(&mut 0.0, y);
            }

            SegmentData::CurveTo {
                ref mut x1,
                ref mut y1,
                ref mut x2,
                ref mut y2,
                ref mut x,
                ref mut y,
            } => {
                ts.apply_ref(x1, y1);
                ts.apply_ref(x2, y2);
                ts.apply_ref(x, y);
            }

            SegmentData::SmoothCurveTo {
                ref mut x2,
                ref mut y2,
                ref mut x,
                ref mut y,
            } => {
                ts.apply_ref(x2, y2);
                ts.apply_ref(x, y);
            }

            SegmentData::Quadratic {
                ref mut x1,
                ref mut y1,
                ref mut x,
                ref mut y,
            } => {
                ts.apply_ref(x1, y1);
                ts.apply_ref(x, y);
            }

            SegmentData::EllipticalArc {
                ref mut rx,
                ref mut ry,
                ref mut x,
                ref mut y,
                ..
            } => {
                *rx *= sx;
                *ry *= sx;

                ts.apply_ref(x, y);
            }

            SegmentData::ClosePath => {}
        }
    }
}

#[cfg(test)]
mod tests {
    use std::str::FromStr;

    use super::*;
    use svgdom::path::Path;
    use svgdom::Transform;

    macro_rules! test {
        ($name:ident, $in_path:expr, $in_ts:expr, $out_text:expr) => {
            #[test]
            fn $name() {
                let mut path = Path::from_str($in_path).unwrap();
                path.conv_to_absolute();

                let ts = Transform::from_str($in_ts).unwrap();

                apply_transform(&mut path, &ts);
                assert_eq_text!(path.to_string(), $out_text);
            }
        };
    }

    test!(
        apply_1,
        "M 10 20 L 30 40",
        "translate(10 20)",
        "M 20 40 L 40 60"
    );
}
