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

use clap::ArgMatches;

use svgdom::{Document, AttributeValue};
use svgdom::types::path::Path;

use task::short::{EId, AId};
use cli::{KEYS, Key};

mod rm_unused;

pub fn process_paths(doc: &Document, args: &ArgMatches) {
    for node in doc.descendants().filter(|n| n.is_tag_id(EId::Path)) {
        // We can't process paths with marker, because if we remove all segments
        // it will break rendering.
        // TODO: do not remove first segment if node has marker
        let has_marker = node.has_attributes(&[AId::Marker, AId::MarkerStart,
                                               AId::MarkerMid, AId::MarkerEnd]);

        let mut attrs = node.attributes_mut();
        match attrs.get_mut(AId::D) {
            Some(attr) => {
                match attr.value {
                    AttributeValue::Path(ref mut path) => process_path(path, has_marker, args),
                    _ => {}
                }
            }
            None => {}
        }
    }
}

fn process_path(path: &mut Path, has_marker: bool, args: &ArgMatches) {
    path.to_absolute();

    if get_flag!(args, Key::RemoveUnusedSegments) && !has_marker {
        rm_unused::remove_unused_segments(path);
    }

    path.to_relative();

    // NOTE: A relative path can be bigger, but usually only on few chars.
    //       But to check it - we need to convert an original and a new path to strings
    //       and compare theme. And in case that new path is bigger - fallback to original,
    //       so we need a copy of original path too. It will be extremely slow. Straightforward
    //       implementation gives us 2x slowdown of the whole application time.
    //       Which is unacceptable.
    //       So we sacrifice a very small amount of cleaning ratio for performance benefits.
}
