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

use svgdom::Document;

pub fn remove_unreferenced_ids(doc: &Document) {
    for mut node in doc.descendants() {
        if node.has_id() && !node.is_used() {
            node.set_id(String::new());
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use svgdom::Document;
    use svgdom::ToStringWithOptions;

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => {
            base_test!($name, remove_unreferenced_ids, $in_text, $out_text);
        };
    }

    test!(
        test_1,
        "<svg>
    <radialGradient id='lg1'/>
    <radialGradient id='lg2'/>
    <rect id='unused' fill='url(#lg1)'/>
</svg>",
        "<svg>
    <radialGradient id='lg1'/>
    <radialGradient/>
    <rect fill='url(#lg1)'/>
</svg>
"
    );
}
