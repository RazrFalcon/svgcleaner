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

pub use self::preclean_checks::preclean_checks;
pub use self::rm_unref_ids::remove_unreferenced_ids;
pub use self::trim_ids::trim_ids;
pub use self::group_defs::group_defs;
pub use self::resolve_inherit::resolve_inherit;
pub use self::rm_unused_defs::remove_unused_defs;
pub use self::conv_shapes::convert_shapes_to_paths;
pub use self::fix_attrs::fix_invalid_attributes;
pub use self::final_fixes::{final_fixes, fix_xmlns_attribute};
pub use self::rm_dupl_defs::{remove_dupl_linear_gradients, remove_dupl_radial_gradients};
pub use self::resolve_attrs::resolve_attributes;
pub use self::rm_default_attrs::remove_default_attributes;
pub use self::rm_text_attrs::remove_text_attributes;
pub use self::rm_unused_coords::remove_unused_coordinates;
pub use self::ungroup_groups::ungroup_groups;
pub use self::merge_gradients::merge_gradients;

#[macro_use]
mod macros;
mod conv_shapes;
mod final_fixes;
mod fix_attrs;
mod group_defs;
mod preclean_checks;
mod resolve_attrs;
mod resolve_inherit;
mod rm_default_attrs;
mod rm_dupl_defs;
mod rm_unref_ids;
mod rm_unused_coords;
mod rm_unused_defs;
mod rm_text_attrs;
mod trim_ids;
mod ungroup_groups;
mod merge_gradients;

/// Shorthand names for modules.
mod short {
    pub use svgdom::types::LengthUnit as Unit;
    pub use svgdom::ElementId as EId;
    pub use svgdom::AttributeId as AId;
}
