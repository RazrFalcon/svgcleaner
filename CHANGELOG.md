# Change Log
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

## [Unreleased]
### Fixed
- Groups removing with transform and non-SVG child.
- Transform to path applying when a path has a style defined in a parent element.

## [0.9.2] - 2017-12-16
### Added
- `--group-by-style` can group two nodes now.
- `--group-by-style` can group into `svg` now.
- `defs` element is always the first child of an `svg` now.

### Fixed
- Groups ungrouping inside a `switch` element.
- Complex groups ungrouping.
- `--group-by-style` recursive processing.
- Crash if path has an implicit MoveTo after ClosePath.
- `--remove-unresolved-classes` option had no effect.

## [0.9.1] - 2017-06-18
### Fixed
- `transform` grouping in `--group-by-style`.

### Other
- Includes [svgdom 0.6.0](https://github.com/RazrFalcon/libsvgdom/blob/master/CHANGELOG.md#060---2017-06-18) fixes.
- Includes [svgparser 0.4.1](https://github.com/RazrFalcon/libsvgparser/blob/master/CHANGELOG.md#041---2017-06-15) fixes.

## [0.9.0] - 2017-06-05
### Added
- XML nodes can be indented with tabs now. Using `--indent=tabs`.
- Presentational attributes joining can be forced with `--join-style-attributes=all` now.
- stdin and stdout support.
- `--no-defaults`.
- `--apply-transform-to-paths`.
- `--allow-bigger-file`.
- `--coordinates-precision`.
- `--properties-precision`.
- `--transforms-precision`.
- Better groups with transforms ungrouping.
- `--remove-text-attributes` removes `line-height` and `xml:space` now.
- Some algorithms are moved to the `svgdom` crate.

### Changed
- `<FLAG>` argument type accepts `true`, `false`, `yes`, `no`, `y`, `n` now.
- Flags `--multipass`, `--copy-on-error` and `--quiet` doesn't accepts value now.
- `--indent` accepts only `none`, `0`, `1`, `2`, `3`, `4`, `tabs` now.
- `--join-style-attributes` accepts only `no`, `some`, `all` now.

### Fixed
- `stroke-dasharray` and `stroke-dashoffset` processing during transformations applying.

### Other
- Updated to the latest version of the libsvgdom.

## [0.8.1] - 2017-02-01
### Added
- `--group-by-style` can group by `transform` now.
- `--remove-needless-attributes` and `--remove-default-attributes`
  now supports `overflow` attribute.
- svgcleaner can be used as a library now.

### Fixed
- Incorrect grouping in `--group-by-style`.
- Gradients with one `stop` children processing in `--remove-invisible-elements`.

## [0.8.0] - 2017-01-14
### Added
- `--use-implicit-cmds`.
- `--convert-segments`.
- `--resolve-use`.
- `--paths-coordinates-precision`.
- `--group-by-style`.
- `--apply-transform-to-shapes`.

### Fixed
- CLI arguments processing.

### Removed
- `--move-styles-to-group`. Use `--group-by-style` instead.

### Other
- Updated to the latest version of the libsvgdom.

## [0.7.1] - 2016-11-04
### Added
- Check that input file exist.
- *Remove invisible elements* now removes default `feColorMatrix` element.
- `--remove-unresolved-classes`.

### Other
- Updated to the latest version of the libsvgdom.

## [0.7.0] - 2016-10-09
### Added
- `--remove-dupl-fegaussianblur`.
- `--remove-invalid-stops`.
- `--apply-transform-to-gradients`.
- `--ungroup-defs`.
- `--regroup-gradient-stops`.
- `--remove-gradient-attributes`.
- `--multipass`.

### Removed
- `--precision-coordinate`.
- `--precision-transform`.

## [0.6.91] - 2016-09-20
### Added
- `--ungroup-groups`.
- `--merge-gradients`.
- `--remove-invisible-elements`.
- `--remove-version`.
- `--remove-unused-coordinates`.
- `--remove-needless-attributes`.
- `--move-styles-to-group`.
- `--join-style-attributes`.
- `--paths-to-relative`.
- `--remove-unused-segments`.

### Changed
- 20-40% faster.
- ~15% better cleaning ratio.
- Conditional processing attributes are unsupported now.
- `xlink:href` with external data is unsupported now.
- Update to the latest `libsvgdom` which fixed a lot of bugs.
- New `--help` output.

### Fixed
- Fix duplicated gradients removing.

### Other
- A lot of small cleaning optimizations.

## [0.6.90] - 2016-09-09
### Added
- Own SVG parsing implementation.
- Tests for all cleaning options.
- Documentation for all cleaning options.
- `--copy-on-error` option to copy original file to destination on error.
- `--quiet` option to suppress any output except warnings and errors.
- `-remove-title` and `--remove-desc` which was a part of the `--remove-invisible-elts` option.
- `--remove-text-attributes` which was a part of the the `--remove-notappl-atts` options.
- `--remove-xmlns-xlink-attribute` to remove unused `xmlns:xlink` attribute.

### Changed
- Engine rewrite from C++ to Rust.
- Move GUI to separate repository.
- All CLI keys are completely new.
- All CLI keys accepts bool value now.
- Rename `--remove-proc-instr` to `--remove-declarations`.
- Rename `--remove-nonsvg-elts` to `--remove-nonsvg-elements`.
- Rename `--remove-metadata-elts` to `--remove-metadata`.
- `--remove-inkscape-elts`, `--remove-sodipodi-elts`, `--remove-ai-elts`, `--remove-corel-elts`,
  `--remove-msvisio-elts` and `--remove-sketch-elts` are all under
  `--remove-nonsvg-elements` option now.
- `--remove-inkscape-atts`, `--remove-sodipodi-atts`, `--remove-ai-atts`, `--remove-corel-atts`,
  `--remove-msvisio-atts` and `--remove-sketch-atts` are all under
  `--remove-nonsvg-attributes` option now.
- Split `--remove-duplicated-defs` into `--remove-dupl-lineargradient`
  and `--remove-dupl-radialgradient`.
- Rename `--remove-default-atts` to `--remove-default-attributes`.
- Rename `--simplify-transform-matrix` to `--simplify-transforms`.
- Rename `--rrggbb-to-rgb` to `--trim-colors`.
- Rename `--convert-basic-shapes` to `--convert-shapes`.
- Rename `--transform-precision` to `--precision-transform`.
- Rename `--coordinates-precision` to `--precision-coordinate`.
- `--attributes-precision` is a part of the `--precision-coordinate` now.
- Rename `--compact-output` to `--indent` with ability to set a custom indention value.
- Split `--remove-unneeded-symbols` into `--trim-paths`, `--remove-dupl-cmd-in-paths`
  and `--join-arcto-flags`.
- If cleaned file is bigger than original - you will get an error.

### Removed
- `--remove-prolog` since it can't be saved anyway.
- `--remove-gaussian-blur` since it's destructive.
- `--keep-named-ids` since it's useless.
- `--remove-unused-xlinks` since it done automatically now.
- `--join-style-atts` since it's not a cleaning option.
- `--colors-to-rrggbb` since it done automatically now.
- `--sort-defs` since it's not a cleaning option.

## [0.6.2] - 2014-02-16
### Added
- French translation.

### Fixed
- Fix folder scanning in Wizard.
- Some cleaning fixes.

## [0.6.1] - 2014-02-05
### Added
- Italian translation.

### Fixed
- Fix cleaning on locales with decimal mark ','.

## [0.6.0] - 2014-01-31
### Added
- Replacing of equal elements with 'use'.
- Removing of elements out of viewbox.
- Transformation matrices applying.
- Trimming of 'id' attribute.
- Removing of equal 'filter' and 'clipPath' in 'defs'.
- Removing of Sketch namespaced elements and attributes.
- Saving of last cleaning options to GUI.
- File tree to GUI.
- New keys for CLI.

### Changed
- 3 times faster and 10% better cleaning.
- Improved paths processing.
- Improved removing of equal gradients.
- Improved grouping of elements with similar attributes.
- Improved transform matrices processing.
- Improved ungrouping of containers.
- Improved merging of gradients.
- Improved rounding of numbers.
- Improved removing of invisible elements.
- CLI now depends only on QtCore.
- Moved from QtXml to TinyXML2.
- Removed preset files.

## [0.5.1] - 2013-07-01
### Added
- A new function: "Group elements by style properties".
- 'rotate(<rotate-angle> [<cx> <cy>])' matrix processing.

### Fixed
- Fix xmlns:xlink prefix removing.
- Fix stdDeviation processing.
- Fix program freezing with --keep-comments flag.
- Fix processing of --version flag.
- Fix prolog removing.
- Fix style attributes grouping to one attribute.

## [0.5.0] - 2013-06-16
### Changed
- Engine rewritten from Perl to C++(Qt).
- New engine 5-7 times faster than old one.
- Same cleaning compression value.
- Added base cli support (no svgz and folder processing support).

## 0.4.0 - 2012-06-09
### Added
- First public release with Perl backend and Qt GUI.

[Unreleased]: https://github.com/RazrFalcon/svgcleaner/compare/v0.9.2...HEAD
[0.9.2]: https://github.com/RazrFalcon/svgcleaner/compare/v0.9.1...v0.9.2
[0.9.1]: https://github.com/RazrFalcon/svgcleaner/compare/v0.9.0...v0.9.1
[0.9.0]: https://github.com/RazrFalcon/svgcleaner/compare/v0.8.1...v0.9.0
[0.8.1]: https://github.com/RazrFalcon/svgcleaner/compare/v0.8.0...v0.8.1
[0.8.0]: https://github.com/RazrFalcon/svgcleaner/compare/v0.7.1...v0.8.0
[0.7.1]: https://github.com/RazrFalcon/svgcleaner/compare/v0.7.0...v0.7.1
[0.7.0]: https://github.com/RazrFalcon/svgcleaner/compare/v0.6.91...v0.7.0
[0.6.91]: https://github.com/RazrFalcon/svgcleaner/compare/v0.6.90...v0.6.91
[0.6.90]: https://github.com/RazrFalcon/svgcleaner/compare/v0.6.2...v0.6.90
[0.6.2]: https://github.com/RazrFalcon/svgcleaner/compare/v0.6.1...v0.6.2
[0.6.1]: https://github.com/RazrFalcon/svgcleaner/compare/v0.6...v0.6.1
[0.6.0]: https://github.com/RazrFalcon/svgcleaner/compare/v0.5.1...v0.6
[0.5.1]: https://github.com/RazrFalcon/svgcleaner/compare/v0.5...v0.5.1
[0.5.0]: https://github.com/RazrFalcon/svgcleaner/compare/v0.4...v0.5
