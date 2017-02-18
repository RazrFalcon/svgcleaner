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

use super::short::EId;

use svgdom::Document;

use svgdom_utils;

/// We don't remove invalid elements, just make them invisible.
/// Then they can be removed via `remove_invisible_elements`.
pub fn fix_invalid_attributes(doc: &Document) {
    for node in doc.descendants().svg() {
        // descendants() iterates only over svg elements, which all have a tag name
        match node.tag_id().unwrap() {
            EId::Rect => svgdom_utils::fix_rect_attributes(&node),
            EId::Polyline | EId::Polygon => svgdom_utils::fix_poly_attributes(&node),
            EId::LinearGradient | EId::RadialGradient => svgdom_utils::fix_stop_attributes(&node),
            _ => {}
        }
    }
}
