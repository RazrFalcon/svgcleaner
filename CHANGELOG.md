# Change Log
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

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

## [0.4.0] - 2012-06-09
### Added
- First public release with Perl backend and Qt GUI.

[Unreleased]: https://github.com/RazrFalcon/svgcleaner/compare/v0.6.90...HEAD
[0.6.90]: https://github.com/RazrFalcon/svgcleaner/compare/v0.6.2...v0.6.90
[0.6.2]: https://github.com/RazrFalcon/svgcleaner/compare/v0.6.1...v0.6.2
[0.6.1]: https://github.com/RazrFalcon/svgcleaner/compare/v0.6...v0.6.1
[0.6.0]: https://github.com/RazrFalcon/svgcleaner/compare/v0.5.1...v0.6
[0.5.1]: https://github.com/RazrFalcon/svgcleaner/compare/v0.5...v0.5.1
[0.5]: https://github.com/RazrFalcon/svgcleaner/compare/v0.4...v0.5
