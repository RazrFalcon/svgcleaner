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

use std::ops::Index;

use clap::{Arg, App, ArgMatches};

use svgdom::{ParseOptions, WriteOptions};

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
    MergeGradients,
    RegroupGradientStops,
    RemoveInvalidStops,
    RemoveInvisibleElements,

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
    MoveStylesToGroup,
    JoinStyleAttributes,
    ApplyTransformToGradients,
    RemoveUnresolvedClasses,

    PathsToRelative,
    RemoveUnusedSegments,
    TrimPaths,
    JoinArcToFlags,
    RemoveDuplCmdInPaths,

    TrimColors,
    SimplifyTransforms,
    Indent,

    Multipass,
    CopyOnError,
    Quiet,
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
    "merge-gradients",
    "regroup-gradient-stops",
    "remove-invalid-stops",
    "remove-invisible-elements",

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
    "move-styles-to-group",
    "join-style-attributes",
    "apply-transform-to-gradients",
    "remove-unresolved-classes",

    "paths-to-relative",
    "remove-unused-segments",
    "trim-paths",
    "join-arcto-flags",
    "remove-dupl-cmd-in-paths",

    "trim-colors",
    "simplify-transforms",
    "indent",

    "multipass",
    "copy-on-error",
    "quiet",
]);

macro_rules! gen_flag {
    ($key:expr, $long:expr, $flag:expr) => (
        Arg::with_name(KEYS[$key])
            .long(KEYS[$key])
            .help($long)
            .value_name("FLAG")
            .default_value($flag)
            .validator(is_flag)
    )
}

pub fn prepare_app<'a, 'b>() -> App<'a, 'b> {
    debug_assert!(KEYS.0.len() - 1 == Key::Quiet as usize);

    // NOTE: We use custom help output, because `clap` doesn't support
    //       args grouping.

    let mut a = App::new("svgcleaner")
        .help(include_str!("../data/help.txt"))
        .version("0.7.1")
        .arg(Arg::with_name("in-file")
            .help("Input file")
            .required(true)
            .index(1)
            .validator(is_svg))
        .arg(Arg::with_name("out-file")
            .help("Output file")
            .required(true)
            .index(2)
            .validator(is_svg));

    // elements
    a = a.arg(gen_flag!(Key::RemoveComments,
            "Remove XML comments", "true"));
    a = a.arg(gen_flag!(Key::RemoveDeclarations,
            "Remove XML declarations", "true"));
    a = a.arg(gen_flag!(Key::RemoveNonsvgElements,
            "Remove non-SVG elements", "true"));
    a = a.arg(gen_flag!(Key::RemoveUnusedDefs,
            "Remove unused referenced elements", "true"));
    a = a.arg(gen_flag!(Key::ConvertShapes,
            "Convert basic shapes into paths", "true"));
    a = a.arg(gen_flag!(Key::RemoveTitle,
            "Remove 'title' element", "true"));
    a = a.arg(gen_flag!(Key::RemoveDesc,
            "Remove 'desc' element", "true"));
    a = a.arg(gen_flag!(Key::RemoveMetadata,
            "Remove 'metadata' element", "true"));
    a = a.arg(gen_flag!(Key::RemoveDuplLinearGradients,
            "Remove duplicated 'linearGradient' elements", "true"));
    a = a.arg(gen_flag!(Key::RemoveDuplRadialGradients,
            "Remove duplicated 'radialGradient' elements", "true"));
    a = a.arg(gen_flag!(Key::RemoveDuplFeGaussianBlur,
            "Remove duplicated 'feGaussianBlur' elements", "true"));
    a = a.arg(gen_flag!(Key::UngroupGroups,
            "Ungroup groups", "true"));
    a = a.arg(gen_flag!(Key::UngroupDefs,
            "Ungroup 'defs' element", "true"));
    a = a.arg(gen_flag!(Key::MergeGradients,
            "Merge gradients", "true"));
    a = a.arg(gen_flag!(Key::RegroupGradientStops,
            "Regroup gradient 'stop' elements", "true"));
    a = a.arg(gen_flag!(Key::RemoveInvalidStops,
            "Remove invalid 'stop' elements", "true"));
    a = a.arg(gen_flag!(Key::RemoveInvisibleElements,
            "Remove invisible elements", "true"));

    // attributes
    a = a.arg(gen_flag!(Key::RemoveVersion,
            "Remove 'version' and 'baseProfile' attributes", "true"));
    a = a.arg(gen_flag!(Key::RemoveNonsvgAttributes,
            "Remove non-SVG attributes", "true"));
    a = a.arg(gen_flag!(Key::RemoveUnreferencedIds,
            "Remove unreferenced 'id' attributes", "true"));
    a = a.arg(gen_flag!(Key::TrimIds,
            "Trim 'id' attributes", "true"));
    a = a.arg(gen_flag!(Key::RemoveTextAttributes,
            "Remove text-related attributes if there is no text", "true"));
    a = a.arg(gen_flag!(Key::RemoveUnusedCoordinates,
            "Remove unused coordinate attributes", "true"));
    a = a.arg(gen_flag!(Key::RemoveDefaultAttributes,
            "Remove attributes with default values", "true"));
    a = a.arg(gen_flag!(Key::RemoveXmlnsXlinkAttribute,
            "Remove an unused xmlns:xlink attribute", "true"));
    a = a.arg(gen_flag!(Key::RemoveNeedlessAttributes,
            "Remove attributes that doesn't belong to this element", "true"));
    a = a.arg(gen_flag!(Key::RemoveGradientAttributes,
            "Remove inheritable gradient attributes", "false"));
    a = a.arg(gen_flag!(Key::MoveStylesToGroup,
            "Move presentational attributes to the parent group", "true"));
    a = a.arg(gen_flag!(Key::JoinStyleAttributes,
            "Join presentational attributes", "true"));
    a = a.arg(gen_flag!(Key::ApplyTransformToGradients,
            "Apply transformations to gradients", "true"));
    a = a.arg(gen_flag!(Key::RemoveUnresolvedClasses,
            "Remove unresolved classes from 'class' attributes", "true"));

    // paths
    a = a.arg(gen_flag!(Key::PathsToRelative,
            "Convert path's segments into relative one", "true"));
    a = a.arg(gen_flag!(Key::RemoveUnusedSegments,
            "Remove unused path segments", "true"));
    a = a.arg(gen_flag!(Key::TrimPaths,
            "Use compact notation for paths", "true"));
    a = a.arg(gen_flag!(Key::JoinArcToFlags,
            "Join ArcTo flags", "false"));
    a = a.arg(gen_flag!(Key::RemoveDuplCmdInPaths,
            "Remove subsequent segments command from paths", "true"));

    // output
    a = a.arg(gen_flag!(Key::TrimColors,
            "Use #RGB notation", "true"));
    a = a.arg(gen_flag!(Key::SimplifyTransforms,
            "Simplify transform matrices", "true"));
    a = a.arg(Arg::with_name(KEYS[Key::Indent])
            .long(KEYS[Key::Indent])
            .help("Sets XML nodes indent (-1..4)")
            .value_name("INDENT")
            .validator(is_indent)
            .default_value("-1"));

    // other
    a = a.arg(gen_flag!(Key::Multipass,
            "Clean a file multiple times", "false"));
    a = a.arg(gen_flag!(Key::CopyOnError,
            "Copy an original file to the destination on error", "false"));
    a = a.arg(gen_flag!(Key::Quiet,
            "Show only warnings and errors", "false"));

    a
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

fn is_flag(val: String) -> Result<(), String> {
    match val.as_ref() {
        "true" | "false" => Ok(()),
        _ => Err(String::from("Invalid flag value.")),
    }
}

#[macro_export]
macro_rules! get_flag {
    ($args:ident, $key:expr) => (
        value_t!($args, KEYS[$key], bool).unwrap()
    )
}

// I don't know how to check it using `clap`, so here is manual checks.
pub fn check_values(args: &ArgMatches) -> bool {

    fn check_value(args: &ArgMatches, flag: Key, dep: Key) -> bool {
        if !get_flag!(args, dep) && get_flag!(args, flag) {
            println!("Error: You can use '--{}=true' only with '--{}=true'.",
                KEYS[flag], KEYS[dep]);
            return false;
        }
        true
    }

    if !check_value(args, Key::JoinArcToFlags, Key::TrimPaths) {
        return false;
    }

    if !check_value(args, Key::PathsToRelative, Key::RemoveUnusedSegments) {
        return false;
    }

    true
}

pub fn gen_parse_options(args: &ArgMatches) -> ParseOptions {
    let mut opt = ParseOptions::default();

    opt.parse_comments              = !get_flag!(args, Key::RemoveComments);
    opt.parse_declarations          = !get_flag!(args, Key::RemoveDeclarations);
    opt.parse_unknown_elements      = !get_flag!(args, Key::RemoveNonsvgElements);
    opt.parse_unknown_attributes    = !get_flag!(args, Key::RemoveNonsvgAttributes);
    opt.skip_unresolved_classes     =  get_flag!(args, Key::RemoveUnresolvedClasses);
    opt.parse_px_unit = false;

    opt
}

pub fn gen_write_options(args: &ArgMatches) -> WriteOptions {
    let mut opt = WriteOptions::default();

    opt.paths.use_compact_notation = get_flag!(args, Key::TrimPaths);
    opt.paths.remove_duplicated_commands = get_flag!(args, Key::RemoveDuplCmdInPaths);
    opt.paths.join_arc_to_flags = get_flag!(args, Key::JoinArcToFlags);

    opt.simplify_transform_matrices = get_flag!(args, Key::SimplifyTransforms);

    opt.remove_leading_zero = true;

    opt.trim_hex_colors = get_flag!(args, Key::TrimColors);
    opt.indent = value_t!(args, KEYS[Key::Indent], i8).unwrap();

    // opt.write_hidden_attributes = true;

    opt
}
