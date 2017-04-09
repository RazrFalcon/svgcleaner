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

// TODO: split to submodules and suboptions

use svgdom::types::path::{Path, Segment, Command};
use svgdom::types::FuzzyEq;

use super::utils;

pub fn remove_unused_segments(path: &mut Path) {
    // repeat until we have any changes
    let mut is_changed = true;
    while is_changed {
        is_changed = false;

        if path.d.len() == 1 {
            path.d.clear();
            break;
        }

        remove_mm(path, &mut is_changed);
        remove_zz(path, &mut is_changed);
        remove_mz(path, &mut is_changed);
        process_lz(path, &mut is_changed);
    }
}

fn remove_mm(path: &mut Path, is_changed: &mut bool) {
    // Remove continuous MoveTo segments since they are pointless.
    // We only interested in last one.
    let mut i = 1;
    while i < path.d.len() {
        let curr_cmd = path.d[i].cmd();
        if curr_cmd == Command::MoveTo {
            let prev_cmd = path.d[i - 1].cmd();
            if prev_cmd == Command::MoveTo {
                path.d.remove(i - 1);
                i -= 1;
                *is_changed = true;
            }
        }

        i += 1;
    }
}

fn remove_zz(path: &mut Path, is_changed: &mut bool) {
    // Remove continuous ClosePath segments since they are pointless.
    let mut i = 1;
    while i < path.d.len() {
        let curr_cmd = path.d[i].cmd();
        if curr_cmd == Command::ClosePath {
            let prev_cmd = path.d[i - 1].cmd();
            if prev_cmd == Command::ClosePath {
                path.d.remove(i - 1);
                i -= 1;
                *is_changed = true;
            }
        }

        i += 1;
    }
}

fn remove_mz(path: &mut Path, is_changed: &mut bool) {
    // Remove `M Z` pairs. Since this are null paths.
    let mut i = 1;
    while i < path.d.len() {
        let curr_cmd = path.d[i].cmd();
        if curr_cmd == Command::ClosePath {
            let prev_cmd = path.d[i - 1].cmd();
            if prev_cmd == Command::MoveTo {
                path.d.remove(i - 1);
                path.d.remove(i - 1);
                *is_changed = true;

                if i > 1 {
                    i -= 2;
                } else {
                    i = 0;
                }
            }
        }

        i += 1;
    }
}

fn process_lz(path: &mut Path, is_changed: &mut bool) {
    if path.d.is_empty() {
        return;
    }

    let mut i = 1;
    let mut mx = path.d[0].x().unwrap();
    let mut my = path.d[0].y().unwrap();
    while i < path.d.len() {
        let curr_cmd = path.d[i].cmd();

        // If current segment is ClosePath and previous segment is line-based segment
        // which points to previous MoveTo - then this line-based segment is pointless,
        // because ClosePath will render the same line by itself.
        if curr_cmd == Command::ClosePath {
            let prev_i = i - 1;
            let prev_cmd = path.d[prev_i].cmd();
            if is_line_based(prev_cmd) {
                let (x, y) = utils::resolve_xy(path, prev_i);
                if mx.fuzzy_eq(&x) && my.fuzzy_eq(&y) {
                    // remove this line-based segment
                    path.d.remove(prev_i);
                    i -= 1;
                    *is_changed = true;
                    continue;
                }
            }
        }

        // If line-based segment is followed by MoveTo or located at the end of the path
        // and points to previous MoveTo - than we can replace it with ClosePath.
        let is_last = i == path.d.len() - 1 && curr_cmd != Command::ClosePath;
        if curr_cmd == Command::MoveTo || is_last {
            let prev_i = if is_last { i } else { i - 1 };
            let prev_cmd = path.d[prev_i].cmd();
            if is_line_based(prev_cmd) {
                let (x, y) = utils::resolve_xy(path, prev_i);
                if mx.fuzzy_eq(&x) && my.fuzzy_eq(&y) {
                    // replace line-based segment with ClosePath.
                    path.d[prev_i] = Segment::new_close_path();
                    *is_changed = true;
                    continue;
                }
            }
        }

        // Remember last MoveTo.
        if curr_cmd == Command::MoveTo {
            mx = path.d[i].x().unwrap();
            my = path.d[i].y().unwrap();
        }

        i += 1;
    }
}

fn is_line_based(seg: Command) -> bool
{
    match seg {
          Command::LineTo
        | Command::HorizontalLineTo
        | Command::VerticalLineTo => true,
        _ => false,
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::{FromStream};
    use svgdom::types::path::{Path};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let mut path = Path::from_str($in_text).unwrap();
                path.conv_to_absolute();
                remove_unused_segments(&mut path);
                assert_eq_text!(path.to_string(), $out_text);
            }
        )
    }

    test!(rm_dulp_moveto_1, "m 10 10 m 20 20 l 10 20",
                            "M 30 30 L 40 50");

    test!(rm_dulp_moveto_2, "m 10 10 20 20 l 10 20",
                            "M 10 10 L 30 30 L 40 50");

    test!(rm_dulp_moveto_3, "M 10 10 M 10 10 M 10 10 M 10 10 L 15 20",
                            "M 10 10 L 15 20");


    test!(rm_single_1, "M 10 10", "");


    test!(rm_mz_1, "M 10 10 Z", "");

    test!(rm_mz_2, "M 10 10 Z M 10 10 Z M 10 10 Z", "");

    test!(rm_mz_3, "M 10 10 L 15 20 M 10 20 Z M 10 20 L 15 30",
                   "M 10 10 L 15 20 M 10 20 L 15 30");


    test!(rm_z_1, "M 10 10 Z Z Z", "");

    test!(rm_z_2, "M 10 10 L 15 20 Z Z Z M 10 20 L 20 30",
                  "M 10 10 L 15 20 Z M 10 20 L 20 30");


    test!(rm_lz_1, "M 10 10 L 50 50 L 10 10 Z",
                   "M 10 10 L 50 50 Z");

    test!(rm_lz_2, "M 10 10 L 50 50 L 10 10",
                   "M 10 10 L 50 50 Z");

    test!(rm_lz_3, "M 10 10 L 50 50 L 10 10 M 50 50 L 60 60",
                   "M 10 10 L 50 50 Z M 50 50 L 60 60");

    test!(rm_lz_4, "M 10 10 L 50 50 L 10 10 M 50 50 L 50 50",
                   "M 10 10 L 50 50 Z");
}
