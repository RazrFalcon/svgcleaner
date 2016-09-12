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

use clap::{Arg, App, ArgMatches};

use svgdom::{ParseOptions, WriteOptions, ElementId};

use std::ops::Index;

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
    UngroupGroups,

    RemoveNonsvgAttributes,
    RemoveUnreferencedIds,
    TrimIds,
    RemoveTextAttributes,
    RemoveUnusedCoordinates,
    RemoveDefaultAttributes,
    RemoveXmlnsXlinkAttribute,

    TrimPaths,
    RemoveDuplCmdInPaths,
    JoinArcToFlags,

    PrecisionCoordinate,
    PrecisionTransform,
    TrimColors,
    SimplifyTransforms,
    Indent,

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
    "ungroup-groups",

    "remove-nonsvg-attributes",
    "remove-unreferenced-ids",
    "trim-ids",
    "remove-text-attributes",
    "remove-unused-coordinates",
    "remove-default-attributes",
    "remove-xmlns-xlink-attribute",

    "trim-paths",
    "remove-dupl-cmd-in-paths",
    "join-arcto-flags",

    "precision-coordinate",
    "precision-transform",
    "trim-colors",
    "simplify-transforms",
    "indent",

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

    let mut a = App::new("svgcleaner")
        // .help(include_str!("../data/help.txt")) // TODO: use custom help
        .version("0.6.91") // TODO: take from toml
        .about("svgcleaner could help you to clean up yours SVG \
                files from the unnecessary data.")
        .arg(Arg::with_name("in-file")
            .help("File to clean")
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
            "Remove unused elements inside 'defs' element", "true"));
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
    a = a.arg(gen_flag!(Key::UngroupGroups,
            "Ungroup groups", "true"));

    // attributes
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

    // paths
    // TODO: disable paths processing as option
    a = a.arg(gen_flag!(Key::TrimPaths,
            "Use compact notation for paths", "true"));
    a = a.arg(gen_flag!(Key::RemoveDuplCmdInPaths,
            "Remove subsequent segments command from paths", "true"));
    a = a.arg(gen_flag!(Key::JoinArcToFlags,
            "Join ArcTo flags", "false"));

    // output
    // TODO: try smaller default value
    a = a.arg(gen_precision_arg(KEYS[Key::PrecisionCoordinate],
            "Sets numeric precision for coordinates (1..8)", "6"));
    a = a.arg(gen_precision_arg(KEYS[Key::PrecisionTransform],
            "Sets numeric precision for transformations (1..8)", "8"));
    a = a.arg(gen_flag!(Key::TrimColors,
            "Use #RGB notation when possible", "true"));
    a = a.arg(gen_flag!(Key::SimplifyTransforms,
            "Simplify transform matrices when possible", "true"));
    a = a.arg(Arg::with_name(KEYS[Key::Indent])
            .long(KEYS[Key::Indent])
            .help("Sets XML nodes indent (-1..4)")
            .value_name("INDENT")
            .validator(is_indent)
            .default_value("-1"));

    // other
    a = a.arg(gen_flag!(Key::CopyOnError,
            "Copy original file to destination on error", "false"));
    a = a.arg(gen_flag!(Key::Quiet,
            "Show only warnings and errors", "false"));

    a
}

fn gen_precision_arg<'a>(key: &'a str, help: &'a str, def_value: &'a str) -> Arg<'a, 'a> {
    Arg::with_name(key)
        .long(key)
        .help(help)
        .value_name("PRECISION")
        .validator(is_precision)
        .default_value(def_value)
}

fn is_svg(val: String) -> Result<(), String> {
    if val.ends_with(".svg") || val.ends_with(".SVG") {
        Ok(())
    } else {
        Err(String::from("The file format must be SVG."))
    }
}

fn is_precision(val: String) -> Result<(), String> {
    let n = match val.parse::<u8>() {
        Ok(v) => v,
        Err(e) => return Err(format!("{}", e)),
    };

    if n >= 1 && n <= 8 {
        Ok(())
    } else {
        Err(String::from("Invalid precision value."))
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
    // TODO: y/n, yes/no
    if val == "true" || val == "false" {
        Ok(())
    } else {
        Err(String::from("Invalid flag value."))
    }
}

macro_rules! get_flag {
    ($args:ident, $key:expr) => (
        value_t!($args, KEYS[$key], bool).unwrap()
    )
}

// I don't know how to check it using `clap`, so here is manual checks.
pub fn check_values(args: &ArgMatches) -> bool {
    if !get_flag!(args, Key::TrimPaths) && get_flag!(args, Key::JoinArcToFlags) {
        println!("Error: You can use '--{}=true' only with '--{}=true'.",
            KEYS[Key::JoinArcToFlags], KEYS[Key::TrimPaths]);
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
    opt.parse_px_unit = false;

    if get_flag!(args, Key::RemoveTitle) {
        opt.skip_svg_elements.push(ElementId::Title);
    }

    if get_flag!(args, Key::RemoveDesc) {
        opt.skip_svg_elements.push(ElementId::Desc);
    }

    if get_flag!(args, Key::RemoveMetadata) {
        opt.skip_svg_elements.push(ElementId::Metadata);
    }

    opt
}

pub fn gen_write_options(args: &ArgMatches) -> WriteOptions {
    let mut opt = WriteOptions::default();

    opt.paths.use_compact_notation = get_flag!(args, Key::TrimPaths);
    opt.paths.remove_duplicated_commands = get_flag!(args, Key::RemoveDuplCmdInPaths);
    opt.paths.join_arc_to_flags = get_flag!(args, Key::JoinArcToFlags);

    opt.transforms.simplify_matrix = get_flag!(args, Key::SimplifyTransforms);

    opt.numbers.precision_coordinates = value_t!(args, KEYS[Key::PrecisionCoordinate], u8).unwrap();
    opt.numbers.precision_transforms  = value_t!(args, KEYS[Key::PrecisionTransform], u8).unwrap();
    opt.numbers.remove_leading_zero   = true;

    opt.trim_hex_colors = get_flag!(args, Key::TrimColors);
    opt.indent = value_t!(args, KEYS[Key::Indent], i8).unwrap();

    // opt.write_hidden_attributes = true;

    opt
}
