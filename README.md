# svgcleaner

*svgcleaner* could help you to clean up your SVG files from the unnecessary data.

[![Build Status](https://travis-ci.org/RazrFalcon/svgcleaner.svg?branch=master)](https://travis-ci.org/RazrFalcon/svgcleaner)

## Table of Contents

  * [Purpose](#purpose)
  * [Goals](#goals)
  * [Alternatives](#alternatives)
  * [Charts](#charts)
    * [Correctness](#correctness)
    * [Cleaning ratio](#cleaning-ratio)
    * [Cleaning time](#cleaning-time)
    * [Other](#other)
  * [Documentation](#documentation)
  * [Limitations](#limitations)
  * [Usage](#usage)
    * [CLI](#cli)
    * [GUI](#gui)
  * [Downloads](#downloads)
  * [Building](#building)
  * [Contributing and Issues](#contributing-and-issues)
  * [FAQ](#faq)
  * [License](#license)

## Purpose

The main purpose of the *svgcleaner* is to losslessly reduce size of an SVG image, created in a
vector editing application, before publishing.

Usually, more than half of an SVG image data is useless for rendering:
- Temporary data used by the vector editing application.
- Non-optimal SVG structure representation.
- Unused and invisible graphical elements.
- ...

## Goals

1. **Correctness.** *svgcleaner* should not break an SVG file.
1. **Cleaning ratio.** More is better.
1. **Performance.** An average SVG file processing time should be closer to ~1ms on a modern PC.

## Alternatives

There are only one real alternative to the *svgcleaner* - [svgo](https://github.com/svg/svgo).

Despite obvious advantages of the *svgcleaner*, like correctness, cleaning ratio and performance,
which is shown below, there are some more nuances:

1. *svgcleaner* cleans only one SVG file. It doesn't process SVGZ files.
   It doesn't process directories. It doesn't do anything else. Just one task*.
1. *svgcleaner* is strictly lossless. There are no destructing cleaning options.
1. *svgcleaner* is portable. You can build it into a single executable without any external dependency.

\* You can get all of this features using [GUI](https://github.com/RazrFalcon/svgcleaner-gui).

## Charts

See ['Testing notes'](docs/testing_notes.rst) for details.

All stats was collected using latest release version.

### Correctness

*Less is better.*

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.7.1/docs/images/correctness_chart_W3C_SVG_11_TestSuite.svg)

\* Yes, *svgcleaner* 0.6.2 is an absolute garbage on this data set.

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.7.1/docs/images/correctness_chart_oxygen.svg)

### Cleaning ratio

*More is better.*

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.7.1/docs/images/ratio_chart_W3C_SVG_11_TestSuite.svg)

\* And again, *svgcleaner* 0.6.2 is kinda superb, but since it breaks most
of the files - it's pointless. Same goes for *svgo*.

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.7.1/docs/images/ratio_chart_oxygen.svg)

### Cleaning time

*Less is better.*

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.7.1/docs/images/performance_chart_W3C_SVG_11_TestSuite.svg)

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.7.1/docs/images/performance_chart_oxygen.svg)

### Other

| Collection | Size Before (MiB) | Size After (MiB) | Ratio (%) |
|--------------------------|------------------:|-----------------:|----------:|
| [Breeze icons theme](https://github.com/KDE/breeze-icons) | 21.72 | 11.09 | 48.91 |
| [Ardis icons theme](https://github.com/NitruxSA/ardis-icon-theme) | 11.82 | 5.23 | 55.74 |
| [Humanity icons theme](https://wiki.ubuntu.com/Artwork/Incoming/Karmic/Humanity_Icons?action=AttachFile&do=view&target=humanity_2.1.tar.gz) | 8.56 | 3.08 | 64.01 |
| [Open Icon Library](https://sourceforge.net/projects/openiconlibrary/) | 207.45 |69.44 | 66.53 |
| [Elementary icons theme](https://github.com/elementary/icons) | 17.72 | 7.09 | 59.95 |
| [Adwaita icons theme](https://github.com/GNOME/adwaita-icon-theme) | 2.21 | 0.37 | 83.08 |
| [Faience icon theme](https://www.archlinux.org/packages/community/any/faience-icon-theme/) | 22.35 | 11.14 | 50.16 |

You can find more tests, with detailed explanations of errors, [here](docs/extended_testing.rst).

## [Documentation](docs/svgcleaner.rst)

## Limitations

*svgcleaner* shouldn't change your file unless you say it to, but there are still
things that can't be preserved. So even if you disable all cleaning options there are still things
that will be changed:

- Original indent is not preserved.
- All colors will be formatted as #RRGGBB and #RGB.
- DOCTYPE, CDATA will be processed and removed.
- CSS2 support is minimal. Only simple *class* and *group* selectors are supported.
- CSS2 from the `style` element will be extracted and processes. The `style` element will be removed.
- The `style` attribute will be split into attributes.
- The `class` attribute will be processed and removed.
- Paths and transformations will be reformatted.
- `currentColor` and `inherit` attributes values will be resolved.
- Referenced elements will be moved to the `defs` element.
- IRI and FuncIRI attributes that reference non-existing objects will be removed.
- If the `offset` attribute value of the `stop` element represented as percent - it will be
  converted into number.

## Usage

### CLI

```
svgcleaner in.svg out.svg
```

Use `--help` for a list of the cleaning options and the [doc](docs/svgcleaner.rst) for more details.

### GUI

You can get a GUI [here](https://github.com/RazrFalcon/svgcleaner-gui).

## Downloads

You can get prebuild packages [here](https://github.com/RazrFalcon/svgcleaner-gui/releases).

## Building

You need the latest stable [Rust](https://www.rust-lang.org/) compiler.

```bash
cargo build --release
```

If you're a Rust programmer, you can install *svgcleaner* using:

```bash
cargo install svgcleaner
```

## Contributing and Issues

See [CONTRIBUTING.md](CONTRIBUTING.md) for details.

## [FAQ](FAQ.md)

## License

*svgcleaner* is licensed under the [GPL-2.0](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html).
