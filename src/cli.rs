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

use std::ops::Index;
use std::io::{Write, stderr};

use clap::{Arg, App, ArgMatches};

use svgdom::{ParseOptions, WriteOptions};

use options::Options;

#[derive(Clone,Copy)]
pub enum Key {
    RemoveComments,
    RemoveDeclarations,
    RemoveNonsvgElements,
    RemoveUnusedDefs,
    ConvertShapes,
    RemoveTitle,
    RemoveDesc,
    RemoveMetadata,
    RemoveDuplLinearGradients,
    RemoveDuplRadialGradients,
    RemoveDuplFeGaussianBlur,
    UngroupGroups,
    UngroupDefs,
    GroupByStyle,
    MergeGradients,
    RegroupGradientStops,
    RemoveInvalidStops,
    RemoveInvisibleElements,
    ResolveUse,

    RemoveVersion,
    RemoveNonsvgAttributes,
    RemoveUnreferencedIds,
    TrimIds,
    RemoveTextAttributes,
    RemoveUnusedCoordinates,
    RemoveDefaultAttributes,
    RemoveXmlnsXlinkAttribute,
    RemoveNeedlessAttributes,
    RemoveGradientAttributes,
    JoinStyleAttributes,
    ApplyTransformToGradients,
    ApplyTransformToShapes,
    RemoveUnresolvedClasses,

    PathsToRelative,
    RemoveUnusedSegments,
    ConvertSegments,
    TrimPaths,
    JoinArcToFlags,
    RemoveDuplCmdInPaths,
    UseImplicitCommands,

    TrimColors,
    SimplifyTransforms,
    PathsCoordinatesPrecision,
    Indent,

    Multipass,
    CopyOnError,
    Quiet,
    Stdout,
}

pub struct KeysData<'a>(&'a [&'a str]);

impl<'a> Index<Key> for KeysData<'a> {
    type Output = &'a str;

    fn index(&self, _index: Key) -> &&'a str {
        &self.0[_index as usize]
    }
}

pub static KEYS: &'static KeysData<'static> = &KeysData(&[
    "remove-comments",
    "remove-declarations",
    "remove-nonsvg-elements",
    "remove-unused-defs",
    "convert-shapes",
    "remove-title",
    "remove-desc",
    "remove-metadata",
    "remove-dupl-lineargradient",
    "remove-dupl-radialgradient",
    "remove-dupl-fegaussianblur",
    "ungroup-groups",
    "ungroup-defs",
    "group-by-style",
    "merge-gradients",
    "regroup-gradient-stops",
    "remove-invalid-stops",
    "remove-invisible-elements",
    "resolve-use",

    "remove-version",
    "remove-nonsvg-attributes",
    "remove-unreferenced-ids",
    "trim-ids",
    "remove-text-attributes",
    "remove-unused-coordinates",
    "remove-default-attributes",
    "remove-xmlns-xlink-attribute",
    "remove-needless-attributes",
    "remove-gradient-attributes",
    "join-style-attributes",
    "apply-transform-to-gradients",
    "apply-transform-to-shapes",
    "remove-unresolved-classes",

    "paths-to-relative",
    "remove-unused-segments",
    "convert-segments",
    "trim-paths",
    "join-arcto-flags",
    "remove-dupl-cmd-in-paths",
    "use-implicit-cmds",

    "trim-colors",
    "simplify-transforms",
    "paths-coordinates-precision",
    "indent",

    "multipass",
    "copy-on-error",
    "quiet",
    "stdout",
]);

macro_rules! gen_flag {
    ($key:expr, $flag:expr) => (
        Arg::with_name(KEYS[$key])
            .long(KEYS[$key])
            .value_name("FLAG")
            .default_value($flag)
            .validator(is_flag)
    )
}

pub fn prepare_app<'a, 'b>() -> App<'a, 'b> {
    debug_assert!(KEYS.0.len() - 1 == Key::Stdout as usize);

    // NOTE: We use custom help output, because `clap` doesn't support
    //       args grouping.

    App::new("svgcleaner")
        .help(include_str!("../data/help.txt"))
        .version("0.8.1")
        .arg(Arg::with_name("in-file")
            .required(true)
            .index(1)
            .validator(is_svg))
        .arg(Arg::with_name("out-file")
            .required_unless(KEYS[Key::Stdout])
            .index(2)
            .validator(is_svg))
        .arg(Arg::with_name(KEYS[Key::Stdout])
            .short("c")
            .long(KEYS[Key::Stdout]))

        // elements
        .arg(gen_flag!(Key::RemoveComments, "true"))
        .arg(gen_flag!(Key::RemoveDeclarations, "true"))
        .arg(gen_flag!(Key::RemoveNonsvgElements, "true"))
        .arg(gen_flag!(Key::RemoveUnusedDefs, "true"))
        .arg(gen_flag!(Key::ConvertShapes, "true"))
        .arg(gen_flag!(Key::RemoveTitle, "true"))
        .arg(gen_flag!(Key::RemoveDesc, "true"))
        .arg(gen_flag!(Key::RemoveMetadata, "true"))
        .arg(gen_flag!(Key::RemoveDuplLinearGradients, "true"))
        .arg(gen_flag!(Key::RemoveDuplRadialGradients, "true"))
        .arg(gen_flag!(Key::RemoveDuplFeGaussianBlur, "true"))
        .arg(gen_flag!(Key::UngroupGroups, "true"))
        .arg(gen_flag!(Key::UngroupDefs, "true"))
        .arg(gen_flag!(Key::GroupByStyle, "true"))
        .arg(gen_flag!(Key::MergeGradients, "true"))
        .arg(gen_flag!(Key::RegroupGradientStops, "true"))
        .arg(gen_flag!(Key::RemoveInvalidStops, "true"))
        .arg(gen_flag!(Key::RemoveInvisibleElements, "true"))
        .arg(gen_flag!(Key::ResolveUse, "true"))

        // attributes
        .arg(gen_flag!(Key::RemoveVersion, "true"))
        .arg(gen_flag!(Key::RemoveNonsvgAttributes, "true"))
        .arg(gen_flag!(Key::RemoveUnreferencedIds, "true"))
        .arg(gen_flag!(Key::TrimIds, "true"))
        .arg(gen_flag!(Key::RemoveTextAttributes, "true"))
        .arg(gen_flag!(Key::RemoveUnusedCoordinates, "true"))
        .arg(gen_flag!(Key::RemoveDefaultAttributes, "true"))
        .arg(gen_flag!(Key::RemoveXmlnsXlinkAttribute, "true"))
        .arg(gen_flag!(Key::RemoveNeedlessAttributes, "true"))
        .arg(gen_flag!(Key::RemoveGradientAttributes, "false"))
        .arg(gen_flag!(Key::JoinStyleAttributes, "true"))
        .arg(gen_flag!(Key::ApplyTransformToGradients, "true"))
        .arg(gen_flag!(Key::ApplyTransformToShapes, "true"))
        .arg(gen_flag!(Key::RemoveUnresolvedClasses, "true"))

        // paths
        .arg(gen_flag!(Key::PathsToRelative, "true"))
        .arg(gen_flag!(Key::RemoveUnusedSegments, "true"))
        .arg(gen_flag!(Key::ConvertSegments, "true"))
        .arg(gen_flag!(Key::TrimPaths, "true"))
        .arg(gen_flag!(Key::JoinArcToFlags, "false"))
        .arg(gen_flag!(Key::RemoveDuplCmdInPaths, "true"))
        .arg(gen_flag!(Key::UseImplicitCommands, "true"))

        // output
        .arg(gen_flag!(Key::TrimColors, "true"))
        .arg(gen_flag!(Key::SimplifyTransforms, "true"))
        .arg(Arg::with_name(KEYS[Key::PathsCoordinatesPrecision])
            .long(KEYS[Key::PathsCoordinatesPrecision])
            .value_name("NUM")
            .validator(is_precision)
            .default_value("8"))
        .arg(Arg::with_name(KEYS[Key::Indent])
            .long(KEYS[Key::Indent])
            .value_name("INDENT")
            .validator(is_indent)
            .default_value("-1"))

        // other
        .arg(gen_flag!(Key::Multipass, "false"))
        .arg(gen_flag!(Key::CopyOnError, "false"))
        .arg(gen_flag!(Key::Quiet, "false"))
}

fn is_svg(val: String) -> Result<(), String> {
    if val.ends_with(".svg") || val.ends_with(".SVG") {
        Ok(())
    } else {
        Err(String::from("The file format must be SVG."))
    }
}

fn is_indent(val: String) -> Result<(), String> {
    let n = match val.parse::<i8>() {
        Ok(v) => v,
        Err(e) => return Err(format!("{}", e)),
    };

    if n >= -1 && n <= 4 {
        Ok(())
    } else {
        Err(String::from("Invalid indent value."))
    }
}

fn is_precision(val: String) -> Result<(), String> {
    let n = match val.parse::<u8>() {
        Ok(v) => v,
        Err(e) => return Err(format!("{}", e)),
    };

    if n >= 1 && n <= 12 {
        Ok(())
    } else {
        Err(String::from("Invalid precision value."))
    }
}

fn is_flag(val: String) -> Result<(), String> {
    match val.as_ref() {
        "true" | "false" |
        "yes"  | "no" |
        "y"    | "n" => Ok(()),
        _ => Err(String::from("Invalid flag value.")),
    }
}

pub fn get_flag(args: &ArgMatches, key: Key) -> bool {
    match args.value_of(KEYS[key]).unwrap() {
        "true" | "yes" | "y" => true,
        "false" | "no" | "n" => false,
        _ => unreachable!(), // unreachable because we already validated values at is_flag()
    }
}

// I don't know how to check it using `clap`, so here is manual checks.
pub fn check_values(args: &ArgMatches) -> bool {

    fn check_value(args: &ArgMatches, flag: Key, dep: Key) -> bool {
        if !get_flag(args, flag) && get_flag(args, dep) {
            writeln!(stderr(), "Error: You can use '--{}=true' only with '--{}=true'.",
                     KEYS[dep], KEYS[flag]).unwrap();
            return false;
        }
        true
    }

    // TODO: JoinArcToFlags should be automaticaly disabled if TrimPaths is disabled

    if !check_value(args, Key::TrimPaths, Key::JoinArcToFlags) {
        return false;
    }

    if !check_value(args, Key::PathsToRelative, Key::RemoveUnusedSegments) {
        return false;
    }

    if !check_value(args, Key::PathsToRelative, Key::ConvertSegments) {
        return false;
    }

    true
}

pub fn gen_parse_options(args: &ArgMatches) -> ParseOptions {
    let mut opt = ParseOptions::default();

    opt.parse_comments              = !get_flag(args, Key::RemoveComments);
    opt.parse_declarations          = !get_flag(args, Key::RemoveDeclarations);
    opt.parse_unknown_elements      = !get_flag(args, Key::RemoveNonsvgElements);
    opt.parse_unknown_attributes    = !get_flag(args, Key::RemoveNonsvgAttributes);
    opt.skip_unresolved_classes     =  get_flag(args, Key::RemoveUnresolvedClasses);
    opt.parse_px_unit               = false;

    opt
}

pub fn gen_write_options(args: &ArgMatches) -> WriteOptions {
    let mut opt = WriteOptions::default();

    opt.paths.use_compact_notation          = get_flag(args, Key::TrimPaths);
    opt.paths.remove_duplicated_commands    = get_flag(args, Key::RemoveDuplCmdInPaths);
    opt.paths.join_arc_to_flags             = get_flag(args, Key::JoinArcToFlags);
    opt.paths.use_implicit_lineto_commands  = get_flag(args, Key::UseImplicitCommands);

    let paths_precision = value_t!(args, KEYS[Key::PathsCoordinatesPrecision], u8).unwrap();
    opt.paths.coordinates_precision = paths_precision;


    opt.simplify_transform_matrices = get_flag(args, Key::SimplifyTransforms);

    opt.remove_leading_zero = true;

    opt.trim_hex_colors = get_flag(args, Key::TrimColors);
    opt.indent = value_t!(args, KEYS[Key::Indent], i8).unwrap();

    opt
}

pub fn gen_cleaning_options(args: &ArgMatches) -> Options {
    Options {
        remove_unused_defs: get_flag(args, Key::RemoveUnusedDefs),
        convert_shapes: get_flag(args, Key::ConvertShapes),
        remove_title: get_flag(args, Key::RemoveTitle),
        remove_desc: get_flag(args, Key::RemoveDesc),
        remove_metadata: get_flag(args, Key::RemoveMetadata),
        remove_dupl_linear_gradients: get_flag(args, Key::RemoveDuplLinearGradients),
        remove_dupl_radial_gradients: get_flag(args, Key::RemoveDuplRadialGradients),
        remove_dupl_fe_gaussian_blur: get_flag(args, Key::RemoveDuplFeGaussianBlur),
        ungroup_groups: get_flag(args, Key::UngroupGroups),
        ungroup_defs: get_flag(args, Key::UngroupDefs),
        group_by_style: get_flag(args, Key::GroupByStyle),
        merge_gradients: get_flag(args, Key::MergeGradients),
        regroup_gradient_stops: get_flag(args, Key::RegroupGradientStops),
        remove_invalid_stops: get_flag(args, Key::RemoveInvalidStops),
        remove_invisible_elements: get_flag(args, Key::RemoveInvisibleElements),
        resolve_use: get_flag(args, Key::ResolveUse),

        remove_version: get_flag(args, Key::RemoveVersion),
        remove_unreferenced_ids: get_flag(args, Key::RemoveUnreferencedIds),
        trim_ids: get_flag(args, Key::TrimIds),
        remove_text_attributes: get_flag(args, Key::RemoveTextAttributes),
        remove_unused_coordinates: get_flag(args, Key::RemoveUnusedCoordinates),
        remove_default_attributes: get_flag(args, Key::RemoveDefaultAttributes),
        remove_xmlns_xlink_attribute: get_flag(args, Key::RemoveXmlnsXlinkAttribute),
        remove_needless_attributes: get_flag(args, Key::RemoveNeedlessAttributes),
        remove_gradient_attributes: get_flag(args, Key::RemoveGradientAttributes),
        join_style_attributes: get_flag(args, Key::JoinStyleAttributes),
        apply_transform_to_gradients: get_flag(args, Key::ApplyTransformToGradients),
        apply_transform_to_shapes: get_flag(args, Key::ApplyTransformToShapes),

        paths_to_relative: get_flag(args, Key::PathsToRelative),
        remove_unused_segments: get_flag(args, Key::RemoveUnusedSegments),
        convert_segments: get_flag(args, Key::ConvertSegments),
    }
}
