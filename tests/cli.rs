extern crate svgcleaner;

use svgcleaner::cli;

#[test]
fn cli_1() {
    let app = cli::prepare_app();
    let args = app.get_matches_from_safe(&[
        "svgcleaner",
        "--trim-paths=false",
        "--join-arcto-flags=true",
        "in.svg",
        "out.svg",
    ]).unwrap();

    assert!(!cli::check_values(&args));
}

#[test]
fn cli_2() {
    let app = cli::prepare_app();
    let args = app.get_matches_from_safe(&[
        "svgcleaner",
        "--paths-to-relative=false",
        "--remove-unused-segments=true",
        "in.svg",
        "out.svg",
    ]).unwrap();

    assert!(!cli::check_values(&args));
}

#[test]
fn cli_3() {
    let app = cli::prepare_app();
    let args = app.get_matches_from_safe(&[
        "svgcleaner",
        "--paths-to-relative=false",
        "--convert-segments=true",
        "in.svg",
        "out.svg",
    ]).unwrap();

    assert!(!cli::check_values(&args));
}

#[test]
fn default_1() {
    let app = cli::prepare_app();
    let args = app.get_matches_from_safe(&[
        "svgcleaner",
        "in.svg",
        "out.svg",
    ]).unwrap();

    let parse_opt = cli::gen_parse_options(&args);
    let write_opt = cli::gen_write_options(&args);
    let cleaning_opt = cli::gen_cleaning_options(&args);

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
    let app = cli::prepare_app();
    let args = app.get_matches_from_safe(&[
        "svgcleaner",
        "--no-defaults",
        "in.svg",
        "out.svg",
    ]).unwrap();

    let parse_opt = cli::gen_parse_options(&args);
    let write_opt = cli::gen_write_options(&args);
    let cleaning_opt = cli::gen_cleaning_options(&args);

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
    let app = cli::prepare_app();
    let args = app.get_matches_from_safe(&[
        "svgcleaner",
        "--no-defaults",
        "--remove-comments=true",
        "--trim-paths=true",
        "--remove-unused-defs=true",
        "in.svg",
        "out.svg",
    ]).unwrap();

    let parse_opt = cli::gen_parse_options(&args);
    let write_opt = cli::gen_write_options(&args);
    let cleaning_opt = cli::gen_cleaning_options(&args);

    assert_eq!(parse_opt.parse_comments, false);
    assert_eq!(parse_opt.parse_declarations, true);

    assert_eq!(write_opt.paths.use_compact_notation, true);
    assert_eq!(write_opt.paths.remove_duplicated_commands, false);

    assert_eq!(cleaning_opt.remove_unused_defs, true);
    assert_eq!(cleaning_opt.convert_shapes, false);
    assert_eq!(cleaning_opt.remove_gradient_attributes, false);
}
