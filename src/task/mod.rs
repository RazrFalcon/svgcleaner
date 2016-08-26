/****************************************************************************
**
** SVG Cleaner could help you to clean up your SVG files
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
// pub use self::rm_invisible_elems::remove_invisible_elements;
pub use self::fix_attrs::fix_invalid_attributes;
pub use self::final_fixes::{final_fixes, fix_xmlns_attribute};
pub use self::rm_dupl_defs::{remove_dupl_linear_gradients, remove_dupl_radial_gradients};
pub use self::resolve_attrs::resolve_attributes;
pub use self::rm_default_attrs::remove_default_attributes;

#[macro_use]
mod macros;
mod preclean_checks;
mod rm_unref_ids;
mod trim_ids;
mod group_defs;
mod resolve_inherit;
mod rm_unused_defs;
mod conv_shapes;
// mod rm_invisible_elems;
mod fix_attrs;
mod final_fixes;
mod rm_dupl_defs;
mod resolve_attrs;
mod rm_default_attrs;

/// Shorthand names for modules.
mod short {
    pub use svgdom::types::LengthUnit as Unit;
    pub use svgdom::ElementId as EId;
    pub use svgdom::AttributeId as AId;
}
