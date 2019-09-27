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

pub use self::conv_shapes::convert_shapes_to_paths;
pub use self::final_fixes::*;
pub use self::fix_attrs::fix_invalid_attributes;
pub use self::group_by_style::group_by_style;
pub use self::group_defs::group_defs;
pub use self::join_style_attrs::join_style_attributes;
pub use self::merge_gradients::merge_gradients;
pub use self::preclean_checks::preclean_checks;
pub use self::regroup_gradient_stops::regroup_gradient_stops;
pub use self::resolve_gradients::*;
pub use self::resolve_inherit::resolve_inherit;
pub use self::resolve_use::resolve_use;
pub use self::rm_default_attrs::remove_default_attributes;
pub use self::rm_default_transform::remove_default_transform;
pub use self::rm_dupl_defs::*;
pub use self::rm_elems::remove_element;
pub use self::rm_gradient_attrs::remove_gradient_attributes;
pub use self::rm_invalid_stops::remove_invalid_stops;
pub use self::rm_invisible_elems::remove_invisible_elements;
pub use self::rm_needless_attrs::remove_needless_attributes;
pub use self::rm_text_attrs::remove_text_attributes;
pub use self::rm_unref_ids::remove_unreferenced_ids;
pub use self::rm_unused_coords::remove_unused_coordinates;
pub use self::rm_unused_defs::remove_unused_defs;
pub use self::rm_version::remove_version;
pub use self::round_numbers::round_numbers;
pub use self::trim_ids::trim_ids;
pub use self::ungroup_defs::ungroup_defs;
pub use self::ungroup_groups::ungroup_groups;

#[macro_use]
mod macros;

mod conv_shapes;
mod final_fixes;
mod fix_attrs;
mod group_by_style;
mod group_defs;
mod join_style_attrs;
mod merge_gradients;
mod preclean_checks;
mod regroup_gradient_stops;
mod resolve_gradients;
mod resolve_inherit;
mod resolve_use;
mod rm_default_attrs;
mod rm_default_transform;
mod rm_dupl_defs;
mod rm_elems;
mod rm_gradient_attrs;
mod rm_invalid_stops;
mod rm_invisible_elems;
mod rm_needless_attrs;
mod rm_text_attrs;
mod rm_unref_ids;
mod rm_unused_coords;
mod rm_unused_defs;
mod rm_version;
mod round_numbers;
mod trim_ids;
mod ungroup_defs;
mod ungroup_groups;

pub mod apply_transforms;
pub mod paths;
pub mod utils;

// Shorthand names for modules.
mod short {
    pub use svgdom::AttributeId as AId;
    pub use svgdom::ElementId as EId;
    pub use svgdom::LengthUnit as Unit;
}
