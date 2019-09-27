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

#[cfg(test)]
macro_rules! write_opt_for_tests {
    () => {{
        use svgdom::WriteOptions;
        let mut opt = WriteOptions::default();
        opt.use_single_quote = true;
        opt.simplify_transform_matrices = true;
        opt
    }};
}

#[cfg(test)]
macro_rules! base_test {
    ($name:ident, $functor:expr, $in_text:expr, $out_text:expr) => {
        #[test]
        fn $name() {
            let mut doc = Document::from_str($in_text).unwrap();
            $functor(&mut doc);
            assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
        }
    };
}

#[cfg(test)]
macro_rules! assert_eq_text {
    ($left:expr, $right:expr) => {{
        match (&$left, &$right) {
            (left_val, right_val) => {
                if !(*left_val == *right_val) {
                    panic!(
                        "assertion failed: `(left == right)` \
                         \nleft:  `{}`\nright: `{}`",
                        left_val, right_val
                    )
                }
            }
        }
    }};
}

#[cfg(test)]
macro_rules! test_eq {
    ($name:ident, $in_text:expr) => {
        test!($name, $in_text, $in_text);
    };
}

macro_rules! get_value {
    ($attrs:expr, $t:ident, $aid:expr, $def:expr) => {
        if let Some(&AttributeValue::$t(v)) = $attrs.get_value($aid) {
            v
        } else {
            $def
        }
    };
}
