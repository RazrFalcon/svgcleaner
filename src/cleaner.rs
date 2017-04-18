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

use std::fs;
use std::io::{self, Read, Write};

use svgdom;
use svgdom::{Document, ParseOptions, WriteOptions, WriteBuffer, ElementId};

use options::Options;
use task::*;
use error;

pub fn load_stdin() -> Result<String, io::Error> {
    let mut s = String::new();
    let stdin = io::stdin();
    let mut handle = stdin.lock();

    handle.read_to_string(&mut s)?;

    Ok(s)
}

pub fn load_file(path: &str) -> Result<String, io::Error> {
    let mut file = fs::File::open(path)?;
    let length = file.metadata()?.len() as usize;

    let mut s = String::with_capacity(length + 1);
    file.read_to_string(&mut s)?;

    Ok(s)
}

pub fn parse_data(data: &str, opt: &ParseOptions) -> Result<Document, svgdom::Error> {
    Document::from_str_with_opt(data, opt)
}

pub fn clean_doc(doc: &Document, options: &Options, opt: &WriteOptions)
                 -> Result<(), error::Error> {
    preclean_checks(doc)?;

    // NOTE: Order is important.
    //       Methods should not depend on each other, but for performance reasons
    //       they should be executed in this order.

    // Prepare our document.
    // This methods is not optional.
    utils::resolve_gradient_attributes(doc)?;
    svgdom::postproc::resolve_inherit(doc)?;
    fix_invalid_attributes(doc);
    group_defs(doc);

    // Manipulate with tree structure.
    // Do not remove any attributes before this methods
    // since they uses them.

    // independent task, doesn't impact any other tasks
    if options.remove_title {
        remove_element(doc, ElementId::Title);
    }

    // independent task, doesn't impact any other tasks
    if options.remove_desc {
        remove_element(doc, ElementId::Desc);
    }

    // independent task, doesn't impact any other tasks
    if options.remove_metadata {
        remove_element(doc, ElementId::Metadata);
    }

    if options.remove_unused_defs {
        remove_unused_defs(doc);
    }

    // impact only `linearGradient` and `radialGradient`
    if options.remove_invalid_stops {
        remove_invalid_stops(doc);
    }

    if options.apply_transform_to_gradients {
        // apply transform to gradients before processing to simplify duplicates
        // detecting and merging
        apply_transforms::apply_transform_to_gradients(doc);
    }

    if options.remove_dupl_linear_gradients {
        remove_dupl_linear_gradients(doc);
    }

    if options.remove_dupl_radial_gradients {
        remove_dupl_radial_gradients(doc);
    }

    if options.remove_dupl_fe_gaussian_blur {
        remove_dupl_fe_gaussian_blur(doc);
    }

    if options.merge_gradients {
        merge_gradients(doc);
    }

    if options.apply_transform_to_gradients {
        // do it again, because something may changed after gradients processing
        apply_transforms::apply_transform_to_gradients(doc);
    }

    // run before `apply_transform_to_shapes` and `process_paths`,
    // because it impact transform processing
    if options.ungroup_groups {
        ungroup_groups(doc, options);
    }

    // run before 'convert_shapes_to_paths'
    if options.apply_transform_to_shapes {
        apply_transforms::apply_transform_to_shapes(doc);
    }

    // impact only shapes
    if options.convert_shapes {
        convert_shapes_to_paths(doc);
    }

    // NOTE: run before `remove_invisible_elements`, because this method can remove all
    //       segments from the path which makes it invisible.
    if options.paths_to_relative {
        // we only process path's segments if 'PathsToRelative' is enabled
        paths::process_paths(doc, options);
    }

    if options.remove_invisible_elements {
        remove_invisible_elements(doc);
    }

    // impact only `linearGradient` and `radialGradient`
    if options.regroup_gradient_stops {
        regroup_gradient_stops(doc);
    }

    // ungroup again
    if options.ungroup_groups {
        ungroup_groups(doc, options);
    }

    if options.resolve_use {
        resolve_use(doc);
    }

    // now we can remove any unneeded attributes

    if options.remove_default_attributes {
        remove_default_attributes(doc);
    }

    if options.remove_text_attributes {
        remove_text_attributes(doc);
    }

    if options.remove_needless_attributes {
        remove_needless_attributes(doc);
    }

    if options.remove_gradient_attributes {
        remove_gradient_attributes(doc);
    }

    if options.remove_unused_coordinates {
        remove_unused_coordinates(doc);
    }

    // ungroup again
    if options.ungroup_groups {
        ungroup_groups(doc, options);
    }

    // run only after attributes processed, because
    // there is no point in grouping default/unneeded attributes
    if options.group_by_style {
        group_by_style(doc);
    }

    // final fixes
    // list of things that can't break anything

    // independent task, doesn't impact any other tasks
    if options.remove_unreferenced_ids {
        remove_unreferenced_ids(doc);
    }

    // independent task, doesn't impact any other tasks
    if options.trim_ids {
        trim_ids(doc);
    }

    // independent task, doesn't impact any other tasks
    if options.remove_version {
        remove_version(doc);
    }

    // run at last, because it can remove `defs` element which is used by many algorithms
    if options.ungroup_defs {
        ungroup_defs(doc);
    }

    remove_empty_defs(doc);
    fix_xmlns_attribute(doc, options.remove_xmlns_xlink_attribute);

    // NOTE: must be run at last, since it breaks the linking
    join_style_attributes(doc, options.join_style_attributes, opt);

    Ok(())
}

pub fn write_buffer(doc: &Document, opt: &WriteOptions, buf: &mut Vec<u8>) {
    doc.write_buf_opt(opt, buf);
}

pub fn write_stdout(data: &[u8]) -> Result<(), io::Error> {
    io::stdout().write(&data)?;
    Ok(())
}

pub fn save_file(data: &[u8], path: &str) -> Result<(), io::Error> {
    let mut f = fs::File::create(&path)?;
    f.write_all(&data)?;

    Ok(())
}
