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

#[derive(Clone,Copy,PartialEq)]
pub enum StyleJoinMode {
    None,
    Some,
    All,
}

// Documentation can be found in: docs/svgcleaner.rst
pub struct CleaningOptions {
    pub remove_unused_defs: bool,
    pub convert_shapes: bool,
    pub remove_title: bool,
    pub remove_desc: bool,
    pub remove_metadata: bool,
    pub remove_dupl_linear_gradients: bool,
    pub remove_dupl_radial_gradients: bool,
    pub remove_dupl_fe_gaussian_blur: bool,
    pub ungroup_groups: bool,
    pub ungroup_defs: bool,
    pub group_by_style: bool,
    pub merge_gradients: bool,
    pub regroup_gradient_stops: bool,
    pub remove_invalid_stops: bool,
    pub remove_invisible_elements: bool,
    pub resolve_use: bool,

    pub remove_version: bool,
    pub remove_unreferenced_ids: bool,
    pub trim_ids: bool,
    pub remove_text_attributes: bool,
    pub remove_unused_coordinates: bool,
    pub remove_default_attributes: bool,
    pub remove_xmlns_xlink_attribute: bool,
    pub remove_needless_attributes: bool,
    pub remove_gradient_attributes: bool,
    pub join_style_attributes: StyleJoinMode,
    pub apply_transform_to_gradients: bool,
    pub apply_transform_to_shapes: bool,

    pub paths_to_relative: bool,
    pub remove_unused_segments: bool,
    pub convert_segments: bool,
    pub apply_transform_to_paths: bool,

    // 1..12
    pub coordinates_precision: u8,
    // 1..12
    pub properties_precision: u8,
    // 1..12
    pub paths_coordinates_precision: u8,
    // 1..12
    pub transforms_precision: u8,
}

// Should all be 'false'.
impl Default for CleaningOptions {
    fn default() -> CleaningOptions {
        CleaningOptions {
            remove_unused_defs: false,
            convert_shapes: false,
            remove_title: false,
            remove_desc: false,
            remove_metadata: false,
            remove_dupl_linear_gradients: false,
            remove_dupl_radial_gradients: false,
            remove_dupl_fe_gaussian_blur: false,
            ungroup_groups: false,
            ungroup_defs: false,
            group_by_style: false,
            merge_gradients: false,
            regroup_gradient_stops: false,
            remove_invalid_stops: false,
            remove_invisible_elements: false,
            resolve_use: false,

            remove_version: false,
            remove_unreferenced_ids: false,
            trim_ids: false,
            remove_text_attributes: false,
            remove_unused_coordinates: false,
            remove_default_attributes: false,
            remove_xmlns_xlink_attribute: false,
            remove_needless_attributes: false,
            remove_gradient_attributes: false,
            join_style_attributes: StyleJoinMode::None,
            apply_transform_to_gradients: false,
            apply_transform_to_shapes: false,

            paths_to_relative: false,
            remove_unused_segments: false,
            convert_segments: false,
            apply_transform_to_paths: false,

            coordinates_precision: 6,
            properties_precision: 6,
            paths_coordinates_precision: 8,
            transforms_precision: 8,
        }
    }
}
