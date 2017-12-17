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
    AttributeValue,
    Document,
};
use svgdom::types::Transform;
use svgdom::types::path::Path;

use task::short::{EId, AId};
use task::apply_transforms::utils as ts_utils;
use options::CleaningOptions;

mod apply_transform;
mod conv_segments;
mod rm_unused;

pub fn process_paths(doc: &Document, opt: &CleaningOptions) {
    for mut node in doc.descendants().filter(|n| n.is_tag_name(EId::Path)) {
        // We can't process paths with marker, because if we remove all segments
        // it will break rendering.
        //
        // TODO: do not remove first segment if node has marker
        let has_marker = node.has_attributes(&[AId::Marker, AId::MarkerStart,
                                               AId::MarkerMid, AId::MarkerEnd]);

        let mut ts = None;
        if opt.apply_transform_to_paths {
            if node.has_attribute(AId::Transform) {
                let tsl = ts_utils::get_ts(&node);

                if    ts_utils::is_valid_transform(&tsl)
                   && ts_utils::is_valid_attrs(&node) {
                    ts = Some(tsl);

                    node.remove_attribute(AId::Transform);

                    if tsl.has_scale() {
                        // TODO: maybe ignore paths without 'stroke-width'

                        // We must update 'stroke-width' if transform had scale part in it.
                        let (sx, _) = tsl.get_scale();
                        ::task::utils::recalc_stroke(&mut node, sx);
                    }
                }
            }
        }

        let mut attrs = node.attributes_mut();
        if let Some(&mut AttributeValue::Path(ref mut path)) = attrs.get_value_mut(AId::D) {
            process_path(path, has_marker, ts, opt);
        }
    }
}

fn process_path(path: &mut Path, has_marker: bool, ts: Option<Transform>, opt: &CleaningOptions) {
    if path.d.is_empty() {
        return;
    }

    path.conv_to_absolute();

    conv_segments::fix_m(path);
    conv_segments::convert_hv_to_l(path);

    if opt.convert_segments {
        conv_segments::convert_segments(path);
    }

    if opt.remove_unused_segments && !has_marker {
        rm_unused::remove_unused_segments(path);

        if path.d.is_empty() {
            return;
        }
    }

    if let Some(ref ts) = ts {
        apply_transform::apply_transform(path, ts);
    }

    conv_segments::convert_l_to_hv(path);

    path.conv_to_relative();

    // TODO: estimate path length
    // NOTE: A relative path can be bigger, but usually only on few chars.
    //       But to check it - we need to convert an original and a new path to strings
    //       and compare theme. And in case that new path is bigger - fallback to original,
    //       so we need a copy of original path too. It will be extremely slow. Straightforward
    //       implementation gives us 2x slowdown of the whole application time.
    //       Which is unacceptable.
    //       So we sacrifice a very small amount of cleaning ratio for performance benefits.
}

mod utils {
    use svgdom::types::path::{Path, Command};

    // HorizontalLineTo, VerticalLineTo and ClosePath does not have 'x'/'y' coordinates,
    // so we have to find them in previous segments.
    pub fn resolve_xy(path: &Path, start: usize) -> (f64, f64) {
        let mut i = start;
        loop {
            let seg = &path.d[i];

            // H and V should be already converted into L,
            // so we check only for Z.
            if seg.cmd() != Command::ClosePath {
                return (seg.x().unwrap(), seg.y().unwrap());
            }

            if i == 0 {
                break;
            }

            i -= 1;
        }

        unreachable!("first segment must be MoveTo");
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use options::CleaningOptions;
    use svgdom::{Document, ToStringWithOptions};

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_str($in_text).unwrap();

                let mut opt = CleaningOptions::default();
                opt.paths_to_relative = true;
                opt.remove_unused_segments = true;
                opt.convert_segments = true;
                opt.apply_transform_to_paths = true;

                process_paths(&doc, &opt);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    test!(empty,
"<svg>
    <path d=''/>
</svg>",
"<svg>
    <path d=''/>
</svg>
");

    test!(single,
"<svg>
    <path d='M 10 20'/>
</svg>",
"<svg>
    <path d=''/>
</svg>
");

    test!(invalid,
"<svg>
    <path d='M'/>
    <path d='M 10'/>
</svg>",
"<svg>
    <path d=''/>
    <path d=''/>
</svg>
");

    test!(transform_1,
"<svg>
    <path id='valid' d='M 10 20 L 30 40' transform='translate(10 20)'/>
    <path id='valid2' d='M 10 20 L 30 40' transform='scale(2)'/>
    <path id='ignored' d='M 10 20 L 30 40' transform='rotate(30)'/>
</svg>",
"<svg>
    <path id='valid' d='m 20 40 l 20 20'/>
    <path id='valid2' d='m 20 40 l 40 40' stroke-width='2'/>
    <path id='ignored' d='m 10 20 l 20 20' transform='rotate(30)'/>
</svg>
");

    // Skip transform if style was defined in the parent node
    test!(transform_2,
"<svg>
    <linearGradient id='lg1'/>
    <g fill='url(#lg1)'>
        <path id='valid' d='M 10 20 L 30 40' transform='translate(10 20)'/>
    </g>
</svg>",
"<svg>
    <linearGradient id='lg1'/>
    <g fill='url(#lg1)'>
        <path id='valid' d='m 10 20 l 20 20' transform='translate(10 20)'/>
    </g>
</svg>
");

    test!(marker,
"<svg>
    <marker id='m'/>
    <path d='M 10 20 L 30 40' marker='url(#m)'/>
    <path d='M 10 20' marker='url(#m)'/>
    <path d='M 10 20'/>
</svg>",
"<svg>
    <marker id='m'/>
    <path d='m 10 20 l 20 20' marker='url(#m)'/>
    <path d='m 10 20' marker='url(#m)'/>
    <path d=''/>
</svg>
");
}
