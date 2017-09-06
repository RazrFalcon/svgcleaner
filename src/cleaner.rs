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

use std::fs;
use std::io::{
    self,
    Read,
    Write,
};

use svgdom::{
    self,
    Document,
    ElementId,
    ParseOptions,
    WriteBuffer,
    WriteOptions,
};

use options::CleaningOptions;
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

pub fn clean_doc(doc: &mut Document, options: &CleaningOptions, opt: &WriteOptions)
                 -> Result<(), error::Error> {
    preclean_checks(doc)?;

    // NOTE: Order is important.
    //       Methods should not depend on each other, but for performance reasons
    //       they should be executed in this order.

    // Prepare our document.
    // This methods is not optional.
    resolve_linear_gradient_attributes(doc);
    resolve_radial_gradient_attributes(doc);
    resolve_stop_attributes(doc)?;

    resolve_inherit(doc);
    fix_invalid_attributes(doc);
    group_defs(doc);

    // Round numbers before everything else.
    round_numbers(doc, options);

    // Manipulate with tree structure.
    // Do not remove any attributes before this methods
    // since they uses them.

    // Independent task, doesn't impact any other tasks..
    if options.remove_title {
        remove_element(doc, ElementId::Title);
    }

    // Independent task, doesn't impact any other tasks.
    if options.remove_desc {
        remove_element(doc, ElementId::Desc);
    }

    // Independent task, doesn't impact any other tasks.
    if options.remove_metadata {
        remove_element(doc, ElementId::Metadata);
    }

    if options.remove_unused_defs {
        remove_unused_defs(doc);
    }

    // Impact only 'linearGradient' and 'radialGradient'.
    if options.remove_invalid_stops {
        remove_invalid_stops(doc);
    }

    if options.apply_transform_to_gradients {
        // Apply transform to gradients before processing to simplify duplicates
        // detecting and merging.
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
        // Do it again, because something may changed after gradients processing.
        apply_transforms::apply_transform_to_gradients(doc);
    }

    // Run before 'apply_transform_to_shapes' and 'process_paths',
    // because it impact transform processing.
    if options.ungroup_groups {
        ungroup_groups(doc, options);
    }

    // Run before 'convert_shapes_to_paths'.
    if options.apply_transform_to_shapes {
        apply_transforms::apply_transform_to_shapes(doc);
    }

    // Impact only shapes.
    if options.convert_shapes {
        convert_shapes_to_paths(doc);
    }

    // NOTE: Run before 'remove_invisible_elements', because this method can remove all
    //       segments from the path which makes it invisible.
    if options.paths_to_relative {
        // We only process path's segments if 'PathsToRelative' is enabled.
        paths::process_paths(doc, options);
    }

    if options.remove_invisible_elements {
        remove_invisible_elements(doc);
    }

    // Impact only 'linearGradient' and 'radialGradient'.
    if options.regroup_gradient_stops {
        regroup_gradient_stops(doc);
    }

    // Ungroup again.
    if options.ungroup_groups {
        ungroup_groups(doc, options);
    }

    if options.resolve_use {
        resolve_use(doc);
    }

    // Now we can remove any unneeded attributes.

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

    // Ungroup again.
    if options.ungroup_groups {
        ungroup_groups(doc, options);
    }

    // Run only after attributes processed, because
    // there is no point in grouping default/unneeded attributes.
    if options.group_by_style {
        group_by_style(doc, opt);
    }

    // Final fixes.
    // List of things that can't break anything.

    // Independent task, doesn't impact any other tasks.
    if options.remove_unreferenced_ids {
        remove_unreferenced_ids(doc);
    }

    // Independent task, doesn't impact any other tasks.
    if options.trim_ids {
        trim_ids(doc);
    }

    // Independent task, doesn't impact any other tasks.
    if options.remove_version {
        remove_version(doc);
    }

    // Run at last, because it can remove 'defs' element which is used by many algorithms.
    if options.ungroup_defs {
        ungroup_defs(doc);
    }

    remove_empty_defs(doc);
    fix_xmlns_attribute(doc, options.remove_xmlns_xlink_attribute);

    // NOTE: Must be run at last, since it breaks the linking.
    // TODO: should be avoided during multipass
    join_style_attributes(doc, options.join_style_attributes, opt);

    Ok(())
}

pub fn write_buffer(doc: &Document, opt: &WriteOptions, buf: &mut Vec<u8>) {
    doc.write_buf_opt(opt, buf);
}

pub fn write_stdout(data: &[u8]) -> Result<(), io::Error> {
    io::stdout().write_all(data)?;
    Ok(())
}

pub fn save_file(data: &[u8], path: &str) -> Result<(), io::Error> {
    let mut f = fs::File::create(path)?;
    f.write_all(data)?;

    Ok(())
}
