# svgcleaner

*svgcleaner* could help you to clean up your SVG files from the unnecessary data.

## Table of contents

  * [Goals](#goals)
    * [Charts](#charts)
      * [Correctness](#correctness)
      * [Cleaning ratio](#cleaning-ratio)
      * [Performance](#performance)
      * [Notes](#notes)
  * [Building](#building)
  * [Usage](#usage)
    * [CLI](#cli)
    * [GUI](#gui)
  * [Downloads](#downloads)
  * [Roadmap](#roadmap)
  * [License](#license)

## Goals

1. **Correctness.** svgcleaner should not break an SVG file.
1. **Cleaning ratio.** More is better.
1. **Performance.** If something can be faster - it should be faster.
   An average SVG file should be processed by less than 16ms on a modern PC.

### Charts

There are only one alternative to svgcleaner - [svgo](https://github.com/svg/svgo),
so we will compare with it.

#### Correctness

*Less is better.*

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.6.90/data/correctness_chart_W3C_SVG_11_TestSuite.svg)

\* Yes, svgcleaner 0.6.2 is absolute garbage on this data set.

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.6.90/data/correctness_chart_oxygen.svg)

#### Cleaning ratio

*More is better.*

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.6.90/data/ratio_chart_W3C_SVG_11_TestSuite.svg)

\* And again, svgcleaner 0.6.2 is kinda superb, but since it breaks most
of the files - it's pointless.

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.6.90/data/ratio_chart_oxygen.svg)

#### Performance

*Less is better.*

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.6.90/data/performance_chart_W3C_SVG_11_TestSuite.svg)

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.6.90/data/performance_chart_oxygen.svg)

#### Notes

 - PC: i5-3570k@4.2GHz, Gentoo Linux Stable x86_64.

 - Input files are stored on HDD, cleaned files was saved to the `tmpfs`.

 - I know that a performance comparison is not fair since `svgo` have to restart nodejs
each time. But I don't know how to prevent it or ignore nodejs starting time.

 - New `svgcleaner` was running using default options.

 - Old `svgcleaner` was running using default options with numeric `--*-precision=6`
   and `--create-viewbox` disabled.

 - `svgo` was running using default options with `--precision=6`.

 - Render error in tests above indicates that file has more then 2% of changed pixels.
   All images are rendered using QtWebKit in original resolution.

 - Used nodejs: `net-libs/nodejs-4.4.6(icu npm ssl -debug -snapshot -test
CPU_FLAGS_X86="sse2" PYTHON_TARGETS="python2_7")`.
   I tried the latest version, 6.4.0, but it even slower.

 - You can find links to the data sets [here](tools/files-testing/README.md).

 - You can repeat tests by yourself using [stats](tools/stats) app.

## Building

You need the latest stable [Rust](https://www.rust-lang.org/) compiler.

```bash
cargo build --release
```

## Usage

### CLI

```
svgcleaner in.svg out.svg
```

Use `--help` for a list of the cleaning options and the [doc](docs/svgcleaner.rst) for more details.

### GUI

You can get a GUI [here](https://github.com/RazrFalcon/svgcleaner-gui).

## Downloads

You can get a prebuild packages [here](https://github.com/RazrFalcon/svgcleaner-gui/releases).

## Roadmap
V0.7
 - [x] Remove text related-attributes if there is no text.
 - [x] Ungroup groups.
 - [ ] Remove duplicated defs:
   - [ ] `feGaussianBlur`
   - [ ] `clipPath`
   - [ ] other...
 - [ ] Remove invisible elements.
   - [x] Invisible elements inside the `clipPath`.
   - [ ] other...
 - [ ] Remove needless attributes.
   - [x] Remove attributes which does not belong to this element.
     - [x] Basic shapes
     - [ ] other...
   - [x] Remove presentational attributes from elements inside `clipPath`.
   - [ ] other...
 - [ ] Group elements by the style attributes.
 - [x] Remove `version` attribute.
 - [ ] Remove elements outside the viewbox.
 - [x] Merge gradients.
 - [ ] Apply transforms to shapes:
   - [ ] paths
   - [ ] shapes
   - [ ] gradients
 - [ ] Replace equal elements by `use`:
   - [ ] `path`
   - [ ] Basic shapes
   - [ ] other...
 - [ ] Group text styles.
 - [ ] Process paths:
    - [x] segments to relative
    - [ ] remove ClosePath segments
    - [ ] remove unneeded MoveTo segments
    - [ ] remove tiny segments
    - [ ] convert segments to shorter one
    - [ ] join segments
 - [ ] Join font properties into the `font` attribute.
 - [ ] Join sequential paths.
 - [ ] Join presentational attributes.
 - [ ] Process `enable-background`.

V0.8
 - [ ] Convert units using specified DPI.
 - [ ] Numbers comparing using custom precision.
 - [ ] Unix pipes.

Note, that current git version is pretty stable and ready to use.

## License

*svgcleaner* is licensed under the [GPL-2.0](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html).
