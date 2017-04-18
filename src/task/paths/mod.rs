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

use svgdom::{Document, AttributeValue};
use svgdom::types::path::Path;
use svgdom::types::Transform;

use task::short::{EId, AId};
use task::apply_transforms::utils as ts_utils;
use options::Options;

mod apply_transform;
mod conv_segments;
mod rm_unused;

pub fn process_paths(doc: &Document, options: &Options) {
    for node in doc.descendants().svg().filter(|n| n.is_tag_name(EId::Path)) {
        // we can't process paths with marker, because if we remove all segments
        // it will break rendering
        //
        // TODO: do not remove first segment if node has marker
        let has_marker = node.has_attributes(&[AId::Marker, AId::MarkerStart,
                                               AId::MarkerMid, AId::MarkerEnd]);

        let mut ts = None;
        if options.apply_transform_to_paths {
            if node.has_attribute(AId::Transform) {
                let tsl = ts_utils::get_ts(&node);

                if    ts_utils::is_valid_transform(&tsl)
                   && ts_utils::is_valid_attrs(&node) {
                    ts = Some(tsl);

                    node.remove_attribute(AId::Transform);

                    if tsl.has_scale() {
                        // we must update 'stroke-width' if transform had scale part in it
                        let (sx, _) = tsl.get_scale();
                        ::task::utils::recalc_stroke(&node, sx);
                    }
                }
            }
        }

        let mut attrs = node.attributes_mut();
        if let Some(&mut AttributeValue::Path(ref mut path)) = attrs.get_value_mut(AId::D) {
            process_path(path, has_marker, ts, options);
        }
    }
}

fn process_path(path: &mut Path, has_marker: bool, ts: Option<Transform>, options: &Options) {
    path.conv_to_absolute();

    if options.convert_segments {
        conv_segments::convert_segments(path);
    }

    if options.remove_unused_segments && !has_marker {
        rm_unused::remove_unused_segments(path);
    }

    if let Some(ref ts) = ts {
        apply_transform::apply_transform(path, ts);
    }

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
    use svgdom::types::path::{Path};

    // TODO: replace with a macro
    pub fn resolve_x(path: &Path, start: usize) -> f64 {
        // VerticalLineTo does not have `x` coordinate, so we have to find it in previous segments
        let mut i = start;
        loop {
            if let Some(x) = path.d[i].x() {
                return x;
            }

            if i == 0 {
                break;
            }
            i -= 1;
        }

        // first segment must be MoveTo, so we will always have an `x`
        unreachable!();
    }

    pub fn resolve_y(path: &Path, start: usize) -> f64 {
        // HorizontalLineTo does not have `x` coordinate, so we have to find it in previous segments
        let mut i = start;
        loop {
            if let Some(y) = path.d[i].y() {
                return y;
            }

            if i == 0 {
                break;
            }
            i -= 1;
        }

        // first segment must be MoveTo, so we will always have an `y`
        unreachable!();
    }

    pub fn resolve_xy(path: &Path, start: usize) -> (f64, f64) {
        (resolve_x(path, start), resolve_y(path, start))
    }
}
