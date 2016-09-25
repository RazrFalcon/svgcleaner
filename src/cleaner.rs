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

use std::fs;
use std::io::{Read,Write};
use std::io;

use clap::ArgMatches;

use svgdom::{Document, ParseOptions, WriteOptions, WriteBuffer, Error};

use cli::{KEYS, Key};
use task::*;
use error::CleanerError;

pub fn load_file(path: &str) -> Result<Vec<u8>, io::Error> {
    let mut file = try!(fs::File::open(path));

    let length = try!(file.metadata()).len() as usize;

    let mut v = Vec::with_capacity(length + 1);
    try!(file.read_to_end(&mut v));

    Ok(v)
}

pub fn parse_data(data: &[u8], opt: &ParseOptions) -> Result<Document, Error> {
    match Document::from_data_with_opt(data, opt) {
        Ok(d) => Ok(d),
        Err(e) => Err(e),
    }
}

pub fn clean_doc(doc: &Document, args: &ArgMatches, opt: &WriteOptions) -> Result<(), CleanerError> {
    try!(preclean_checks(doc));

    // NOTE: Order is important.
    //       Methods should not depend on each other, but for performance reasons
    //       they should be executed in this order.

    // Prepare our document.
    // This methods is not optional.
    try!(resolve_attributes(doc));
    try!(resolve_inherit(doc));
    fix_invalid_attributes(doc);
    group_defs(doc);

    // Manipulate with tree structure.
    // Do not remove any attributes before this methods
    // since they uses them.
    if get_flag!(args, Key::RemoveUnusedDefs) {
        remove_unused_defs(doc);
    }

    if get_flag!(args, Key::RemoveInvalidStops) {
        remove_invalid_stops(doc);
    }

    if get_flag!(args, Key::ApplyTransformToGradients) {
        // Apply transform to gradients before processing to simplify duplicates
        // detecting and merging.
        apply_transforms::apply_transform_to_gradients(&doc);
    }

    if get_flag!(args, Key::RemoveDuplLinearGradients) {
        remove_dupl_linear_gradients(doc);
    }

    if get_flag!(args, Key::RemoveDuplRadialGradients) {
        remove_dupl_radial_gradients(doc);
    }

    if get_flag!(args, Key::RemoveDuplFeGaussianBlur) {
        remove_dupl_fe_gaussian_blur(doc);
    }

    if get_flag!(args, Key::MergeGradients) {
        merge_gradients(doc);
    }

    if get_flag!(args, Key::ApplyTransformToGradients) {
        // Do it again, because something may changed after gradient processing.
        apply_transforms::apply_transform_to_gradients(&doc);
    }

    if get_flag!(args, Key::ConvertShapes) {
        convert_shapes_to_paths(doc);
    }

    // NOTE: run before `remove_invisible_elements`, because this method can remove all
    // segments from the path which makes it invisible.
    if get_flag!(args, Key::PathsToRelative) {
        // we only process path's segments if 'PathsToRelative' is enabled
        paths::process_paths(doc, args);
    }

    if get_flag!(args, Key::RemoveInvisibleElements) {
        remove_invisible_elements(doc);
    }

    if get_flag!(args, Key::UngroupGroups) {
        ungroup_groups(doc);
    }

    // now we can remove any unneeded attributes

    if get_flag!(args, Key::RemoveDefaultAttributes) {
        remove_default_attributes(doc);
    }

    if get_flag!(args, Key::RemoveTextAttributes) {
        remove_text_attributes(doc);
    }

    if get_flag!(args, Key::RemoveNeedlessAttributes) {
        remove_needless_attributes(doc);
    }

    if get_flag!(args, Key::MoveStylesToGroup) {
        move_styles_to_group(doc);
    }

    if get_flag!(args, Key::RemoveUnusedCoordinates) {
        remove_unused_coordinates(doc);
    }

    // final fixes
    // list of things that can't break anything

    if get_flag!(args, Key::RemoveUnreferencedIds) {
        remove_unreferenced_ids(doc);
    }

    if get_flag!(args, Key::TrimIds) {
        trim_ids(doc);
    }

    if get_flag!(args, Key::RemoveVersion) {
        remove_version(doc);
    }

    if get_flag!(args, Key::UngroupDefs) {
        ungroup_defs(doc);
    }

    final_fixes(doc);
    fix_xmlns_attribute(doc, get_flag!(args, Key::RemoveXmlnsXlinkAttribute));

    // NOTE: must be run at last, since it breaks linking.
    if get_flag!(args, Key::JoinStyleAttributes) {
        join_style_attributes(doc, opt);
    }

    Ok(())
}

pub fn write_buffer(doc: &Document, capacity: usize, opt: &WriteOptions) -> Vec<u8> {
    let mut ouput_data = Vec::with_capacity(capacity);
    doc.write_buf_opt(opt, &mut ouput_data);
    ouput_data
}

pub fn save_file(data: &[u8], path: &str) -> Result<(), io::Error> {
    let mut f = try!(fs::File::create(&path));
    try!(f.write_all(&data));

    Ok(())
}
