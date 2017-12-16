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

use std::ops::Index;
use std::io::{
    stderr,
    Write,
};

use clap::{
    Arg,
    App,
    ArgMatches,
};

use svgdom::{
    ParseOptions,
    WriteOptions,
    WriteOptionsPaths,
    Indent,
};

use {
    CleaningOptions,
    StyleJoinMode,
};

#[derive(Debug,Clone,Copy,PartialEq)]
pub enum InputFrom<'a> {
    Stdin,
    File(&'a str),
}

#[derive(Debug,Clone,Copy,PartialEq)]
pub enum OutputTo<'a> {
    Stdout,
    File(&'a str),
}

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
    ApplyTransformToPaths,
    TrimPaths,
    JoinArcToFlags,
    RemoveDuplCmdInPaths,
    UseImplicitCommands,

    TrimColors,
    SimplifyTransforms,
    CoordinatesPrecision,
    PropertiesPrecision,
    TransformsPrecision,
    PathsCoordinatesPrecision,
    Indent,

    NoDefaults,
    Multipass,
    AllowBiggerFile,
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
    "apply-transform-to-paths",
    "trim-paths",
    "join-arcto-flags",
    "remove-dupl-cmd-in-paths",
    "use-implicit-cmds",

    "trim-colors",
    "simplify-transforms",
    "coordinates-precision",
    "properties-precision",
    "transforms-precision",
    "paths-coordinates-precision",
    "indent",

    "no-defaults",
    "multipass",
    "allow-bigger-file",
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

macro_rules! gen_precision {
    ($key:expr, $def_value:expr) => (
        Arg::with_name(KEYS[$key])
            .long(KEYS[$key])
            .value_name("NUM")
            .validator(is_precision)
            .default_value($def_value)
    )
}

pub fn prepare_app<'a, 'b>() -> App<'a, 'b> {
    debug_assert_eq!(KEYS.0.len() - 1, Key::Stdout as usize);

    // NOTE: We use custom help output, because 'clap' doesn't support
    //       args grouping.

    App::new("svgcleaner")
        .help(include_str!("../data/help.txt"))
        .version(env!("CARGO_PKG_VERSION"))
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
        .arg(Arg::with_name(KEYS[Key::NoDefaults])
            .long(KEYS[Key::NoDefaults]))
        .arg(Arg::with_name(KEYS[Key::Multipass])
            .long(KEYS[Key::Multipass]))
        .arg(Arg::with_name(KEYS[Key::AllowBiggerFile])
            .long(KEYS[Key::AllowBiggerFile]))
        .arg(Arg::with_name(KEYS[Key::CopyOnError])
            .long(KEYS[Key::CopyOnError]))
        .arg(Arg::with_name(KEYS[Key::Quiet])
            .long(KEYS[Key::Quiet]))

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
        .arg(Arg::with_name(KEYS[Key::JoinStyleAttributes])
            .long(KEYS[Key::JoinStyleAttributes])
            .value_name("VALUE")
            .possible_values(&["no", "some", "all"])
            .default_value("some"))
        .arg(gen_flag!(Key::ApplyTransformToGradients, "true"))
        .arg(gen_flag!(Key::ApplyTransformToShapes, "true"))
        .arg(gen_flag!(Key::RemoveUnresolvedClasses, "true"))

        // paths
        .arg(gen_flag!(Key::PathsToRelative, "true"))
        .arg(gen_flag!(Key::RemoveUnusedSegments, "true"))
        .arg(gen_flag!(Key::ConvertSegments, "true"))
        .arg(gen_flag!(Key::ApplyTransformToPaths, "false"))
        .arg(gen_flag!(Key::TrimPaths, "true"))
        .arg(gen_flag!(Key::JoinArcToFlags, "false"))
        .arg(gen_flag!(Key::RemoveDuplCmdInPaths, "true"))
        .arg(gen_flag!(Key::UseImplicitCommands, "true"))

        // output
        .arg(gen_flag!(Key::TrimColors, "true"))
        .arg(gen_flag!(Key::SimplifyTransforms, "true"))
        .arg(gen_precision!(Key::CoordinatesPrecision, "6"))
        .arg(gen_precision!(Key::PropertiesPrecision, "6"))
        .arg(gen_precision!(Key::TransformsPrecision, "8"))
        .arg(gen_precision!(Key::PathsCoordinatesPrecision, "8"))
        .arg(Arg::with_name(KEYS[Key::Indent])
            .long(KEYS[Key::Indent])
            .value_name("INDENT")
            .possible_values(&["none", "0", "1", "2", "3", "4", "tabs"])
            .default_value("none"))
}

fn is_svg(val: String) -> Result<(), String> {
    if val.ends_with(".svg") || val.ends_with(".SVG") || val == "-" {
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

    if n >= 1 && n <= 12 {
        Ok(())
    } else {
        Err(String::from("Precision should be in range 1..12."))
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

fn get_flag(args: &ArgMatches, key: Key) -> bool {
    match args.value_of(KEYS[key]).unwrap() {
        "true" | "yes" | "y" => true,
        "false" | "no" | "n" => false,
        _ => unreachable!(), // Unreachable because we already validated values at is_flag().
    }
}

// I don't know how to check it using 'clap', so here is manual checks.
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

    if !check_value(args, Key::PathsToRelative, Key::ApplyTransformToPaths) {
        return false;
    }

    true
}

struct Flags<'a> {
    args: &'a ArgMatches<'a>,
    is_no_defaults: bool,
}

impl<'a> Flags<'a> {
    fn new(args: &'a ArgMatches) -> Flags<'a> {
        Flags {
            args: args,
            is_no_defaults: args.is_present(KEYS[Key::NoDefaults]),
        }
    }

    fn resolve(&self, value: &mut bool, key: Key) {
        if let Some(v) = self._resolve(key) {
            *value = v;
        }
    }

    fn resolve_inv(&self, value: &mut bool, key: Key) {
        if let Some(v) = self._resolve(key) {
            *value = !v;
        }
    }

    fn _resolve(&self, key: Key) -> Option<bool> {
        if self.is_no_defaults {
            // If '--no-defaults' flag is set, check that
            // provided flag is actually set by user and not default.
            //
            // Note that 'is_present' will always return 'true',
            // because all the flags has a default value.
            if self.args.occurrences_of(KEYS[key]) != 0 {
                Some(get_flag(self.args, key))
            } else {
                // If flag is not set - keep value unchanged.
                None
            }
        } else {
            Some(get_flag(self.args, key))
        }
    }
}

pub fn gen_parse_options(args: &ArgMatches) -> ParseOptions {
    let mut opt = ParseOptions {
        parse_px_unit: false,
        .. ParseOptions::default()
    };

    let flags = Flags::new(args);

    flags.resolve_inv(&mut opt.parse_comments, Key::RemoveComments);
    flags.resolve_inv(&mut opt.parse_declarations, Key::RemoveDeclarations);
    flags.resolve_inv(&mut opt.parse_unknown_elements, Key::RemoveNonsvgElements);
    flags.resolve_inv(&mut opt.parse_unknown_attributes, Key::RemoveNonsvgAttributes);
    flags.resolve(&mut opt.skip_unresolved_classes, Key::RemoveUnresolvedClasses);

    opt
}

pub fn gen_write_options(args: &ArgMatches) -> WriteOptions {
    // Initial options should be opposite to default ones.
    let mut opt = WriteOptions {
        indent: Indent::Spaces(4),
        attributes_indent: Indent::None,
        use_single_quote: false,
        trim_hex_colors: false,
        write_hidden_attributes: false,
        remove_leading_zero: true,
        paths: WriteOptionsPaths {
            use_compact_notation: false,
            join_arc_to_flags: false,
            remove_duplicated_commands: false,
            use_implicit_lineto_commands: false,
        },
        simplify_transform_matrices: false,
    };

    let flags = Flags::new(args);

    flags.resolve(&mut opt.paths.use_compact_notation, Key::TrimPaths);
    flags.resolve(&mut opt.paths.remove_duplicated_commands, Key::RemoveDuplCmdInPaths);
    flags.resolve(&mut opt.paths.join_arc_to_flags, Key::JoinArcToFlags);
    flags.resolve(&mut opt.paths.use_implicit_lineto_commands, Key::UseImplicitCommands);

    flags.resolve(&mut opt.simplify_transform_matrices, Key::SimplifyTransforms);

    flags.resolve(&mut opt.trim_hex_colors, Key::TrimColors);

    opt.indent = match args.value_of(KEYS[Key::Indent]).unwrap() {
        "none"  => Indent::None,
        "0"     => Indent::Spaces(0),
        "1"     => Indent::Spaces(1),
        "2"     => Indent::Spaces(2),
        "3"     => Indent::Spaces(3),
        "4"     => Indent::Spaces(4),
        "tabs"  => Indent::Tabs,
        _ => unreachable!(), // clap will validate the input.
    };

    opt
}

pub fn gen_cleaning_options(args: &ArgMatches) -> CleaningOptions {
    let flags = Flags::new(args);

    // All cleaning options are disabled by default.
    let mut opt = CleaningOptions::default();

    flags.resolve(&mut opt.remove_unused_defs, Key::RemoveUnusedDefs);
    flags.resolve(&mut opt.convert_shapes, Key::ConvertShapes);
    flags.resolve(&mut opt.remove_title, Key::RemoveTitle);
    flags.resolve(&mut opt.remove_desc, Key::RemoveDesc);
    flags.resolve(&mut opt.remove_metadata, Key::RemoveMetadata);
    flags.resolve(&mut opt.remove_dupl_linear_gradients, Key::RemoveDuplLinearGradients);
    flags.resolve(&mut opt.remove_dupl_radial_gradients, Key::RemoveDuplRadialGradients);
    flags.resolve(&mut opt.remove_dupl_fe_gaussian_blur, Key::RemoveDuplFeGaussianBlur);
    flags.resolve(&mut opt.ungroup_groups, Key::UngroupGroups);
    flags.resolve(&mut opt.ungroup_defs, Key::UngroupDefs);
    flags.resolve(&mut opt.group_by_style, Key::GroupByStyle);
    flags.resolve(&mut opt.merge_gradients, Key::MergeGradients);
    flags.resolve(&mut opt.regroup_gradient_stops, Key::RegroupGradientStops);
    flags.resolve(&mut opt.remove_invalid_stops, Key::RemoveInvalidStops);
    flags.resolve(&mut opt.remove_invisible_elements, Key::RemoveInvisibleElements);
    flags.resolve(&mut opt.resolve_use, Key::ResolveUse);

    flags.resolve(&mut opt.remove_version, Key::RemoveVersion);
    flags.resolve(&mut opt.remove_unreferenced_ids, Key::RemoveUnreferencedIds);
    flags.resolve(&mut opt.trim_ids, Key::TrimIds);
    flags.resolve(&mut opt.remove_text_attributes, Key::RemoveTextAttributes);
    flags.resolve(&mut opt.remove_unused_coordinates, Key::RemoveUnusedCoordinates);
    flags.resolve(&mut opt.remove_default_attributes, Key::RemoveDefaultAttributes);
    flags.resolve(&mut opt.remove_xmlns_xlink_attribute, Key::RemoveXmlnsXlinkAttribute);
    flags.resolve(&mut opt.remove_needless_attributes, Key::RemoveNeedlessAttributes);
    flags.resolve(&mut opt.remove_gradient_attributes, Key::RemoveGradientAttributes);
    flags.resolve(&mut opt.apply_transform_to_gradients, Key::ApplyTransformToGradients);
    flags.resolve(&mut opt.apply_transform_to_shapes, Key::ApplyTransformToShapes);

    opt.join_style_attributes = match args.value_of(KEYS[Key::JoinStyleAttributes]).unwrap() {
        "no"    => StyleJoinMode::None,
        "some"  => StyleJoinMode::Some,
        "all"   => StyleJoinMode::All,
        _ => unreachable!(), // clap will validate the input.
    };

    flags.resolve(&mut opt.paths_to_relative, Key::PathsToRelative);
    flags.resolve(&mut opt.remove_unused_segments, Key::RemoveUnusedSegments);
    flags.resolve(&mut opt.convert_segments, Key::ConvertSegments);
    flags.resolve(&mut opt.apply_transform_to_paths, Key::ApplyTransformToPaths);

    opt.coordinates_precision = value_t!(args, KEYS[Key::CoordinatesPrecision], u8).unwrap();
    opt.properties_precision = value_t!(args, KEYS[Key::PropertiesPrecision], u8).unwrap();
    opt.paths_coordinates_precision
        = value_t!(args, KEYS[Key::PathsCoordinatesPrecision], u8).unwrap();
    opt.transforms_precision   = value_t!(args, KEYS[Key::TransformsPrecision], u8).unwrap();

    opt
}

pub fn input<'a>(args: &'a ArgMatches) -> InputFrom<'a> {
    let in_file = args.value_of("in-file").unwrap();
    let out_file = args.value_of("out-file");

    if in_file == "-" && args.is_present(KEYS[Key::Stdout]) {
        InputFrom::Stdin
    } else if let Some("-") = out_file {
        InputFrom::Stdin
    } else {
        InputFrom::File(in_file)
    }
}

pub fn output<'a>(args: &'a ArgMatches) -> OutputTo<'a> {
    let in_file = args.value_of("in-file").unwrap();
    let out_file = args.value_of("out-file");

    if args.is_present(KEYS[Key::Stdout]) {
        OutputTo::Stdout
    } else if let Some("-") = out_file {
        OutputTo::File(in_file)
    } else {
        OutputTo::File(out_file.unwrap())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn cli_1() {
        let app = prepare_app();
        let args = app.get_matches_from_safe(&[
            "svgcleaner",
            "--trim-paths=false",
            "--join-arcto-flags=true",
            "in.svg",
            "out.svg",
        ]).unwrap();

        assert!(!check_values(&args));
    }

    #[test]
    fn cli_2() {
        let app = prepare_app();
        let args = app.get_matches_from_safe(&[
            "svgcleaner",
            "--paths-to-relative=false",
            "--remove-unused-segments=true",
            "in.svg",
            "out.svg",
        ]).unwrap();

        assert!(!check_values(&args));
    }

    #[test]
    fn cli_3() {
        let app = prepare_app();
        let args = app.get_matches_from_safe(&[
            "svgcleaner",
            "--paths-to-relative=false",
            "--convert-segments=true",
            "in.svg",
            "out.svg",
        ]).unwrap();

        assert!(!check_values(&args));
    }

    #[test]
    fn cli_4() {
        let app = prepare_app();
        let args = app.get_matches_from_safe(&[
            "svgcleaner",
            "--paths-to-relative=false",
            "--apply-transform-to-paths=true",
            "in.svg",
            "out.svg",
        ]).unwrap();

        assert!(!check_values(&args));
    }

    #[test]
    fn default_1() {
        let app = prepare_app();
        let args = app.get_matches_from_safe(&[
            "svgcleaner",
            "in.svg",
            "out.svg",
        ]).unwrap();

        let parse_opt = gen_parse_options(&args);
        let write_opt = gen_write_options(&args);
        let cleaning_opt = gen_cleaning_options(&args);

        assert_eq!(parse_opt.parse_comments, false);
        assert_eq!(parse_opt.parse_declarations, false);

        assert_eq!(write_opt.paths.use_compact_notation, true);
        assert_eq!(write_opt.paths.remove_duplicated_commands, true);

        assert_eq!(cleaning_opt.remove_unused_defs, true);
        assert_eq!(cleaning_opt.convert_shapes, true);
        assert_eq!(cleaning_opt.remove_gradient_attributes, false);
    }

    #[test]
    fn no_defaults_1() {
        let app = prepare_app();
        let args = app.get_matches_from_safe(&[
            "svgcleaner",
            "--no-defaults",
            "in.svg",
            "out.svg",
        ]).unwrap();

        let parse_opt = gen_parse_options(&args);
        let write_opt = gen_write_options(&args);
        let cleaning_opt = gen_cleaning_options(&args);

        assert_eq!(parse_opt.parse_comments, true);
        assert_eq!(parse_opt.parse_declarations, true);

        assert_eq!(write_opt.paths.use_compact_notation, false);
        assert_eq!(write_opt.paths.remove_duplicated_commands, false);

        assert_eq!(cleaning_opt.remove_unused_defs, false);
        assert_eq!(cleaning_opt.convert_shapes, false);
        assert_eq!(cleaning_opt.remove_gradient_attributes, false);
    }

    #[test]
    fn no_defaults_2() {
        let app = prepare_app();
        let args = app.get_matches_from_safe(&[
            "svgcleaner",
            "--no-defaults",
            "--remove-comments=true",
            "--trim-paths=true",
            "--remove-unused-defs=true",
            "in.svg",
            "out.svg",
        ]).unwrap();

        let parse_opt = gen_parse_options(&args);
        let write_opt = gen_write_options(&args);
        let cleaning_opt = gen_cleaning_options(&args);

        assert_eq!(parse_opt.parse_comments, false);
        assert_eq!(parse_opt.parse_declarations, true);

        assert_eq!(write_opt.paths.use_compact_notation, true);
        assert_eq!(write_opt.paths.remove_duplicated_commands, false);

        assert_eq!(cleaning_opt.remove_unused_defs, true);
        assert_eq!(cleaning_opt.convert_shapes, false);
        assert_eq!(cleaning_opt.remove_gradient_attributes, false);
    }

    #[test]
    fn no_std() {
        let app = prepare_app();
        let args = app.get_matches_from_safe(&[
            "svgcleaner",
            "in.svg",
            "out.svg",
        ]).unwrap();

        assert_eq!(input(&args), InputFrom::File("in.svg"));
        assert_eq!(output(&args), OutputTo::File("out.svg"));
    }

    #[test]
    fn std_1() {
        let app = prepare_app();
        let args = app.get_matches_from_safe(&[
            "svgcleaner",
            "-c",
            "-",
        ]).unwrap();

        assert_eq!(input(&args), InputFrom::Stdin);
        assert_eq!(output(&args), OutputTo::Stdout);
    }

    #[test]
    fn std_2() {
        let app = prepare_app();
        let args = app.get_matches_from_safe(&[
            "svgcleaner",
            "-c",
            "in.svg",
        ]).unwrap();

        assert_eq!(input(&args), InputFrom::File("in.svg"));
        assert_eq!(output(&args), OutputTo::Stdout);
    }

    #[test]
    fn std_3() {
        let app = prepare_app();
        let args = app.get_matches_from_safe(&[
            "svgcleaner",
            "out.svg",
            "-",
        ]).unwrap();

        assert_eq!(input(&args), InputFrom::Stdin);
        assert_eq!(output(&args), OutputTo::File("out.svg"));
    }
}
