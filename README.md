# svgcleaner

*svgcleaner* could help you to clean up your SVG files from the unnecessary data.

## Table of contents

  * [Goals](#goals)
    * [Charts](#charts)
      * [Correctness](#correctness)
      * [Cleaning ratio](#cleaning-ratio)
      * [Cleaning time](#cleaning-time)
  * [Limitations](#limitations)
  * [Building](#building)
  * [Usage](#usage)
    * [CLI](#cli)
    * [GUI](#gui)
  * [Downloads](#downloads)
  * [Contributing and Issues](#contributing-and-issues)
  * [Roadmap](#roadmap)
  * [License](#license)

## Goals

1. **Correctness.** svgcleaner should not break an SVG file.
1. **Cleaning ratio.** More is better.
1. **Performance.** If something can be faster - it should be faster.
   An average SVG file processing time should be closer to ~1ms on a modern PC.

### Charts

There are only one alternative to svgcleaner - [svgo](https://github.com/svg/svgo),
so we will compare with it.

See ['Testing notes'](docs/testing_notes.rst) for details.

#### Correctness

*Less is better.*

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.6.91/docs/images/correctness_chart_W3C_SVG_11_TestSuite.svg)

\* Yes, svgcleaner 0.6.2 is absolute garbage on this data set.

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.6.91/docs/images/correctness_chart_oxygen.svg)

#### Cleaning ratio

*More is better.*

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.6.91/docs/images/ratio_chart_W3C_SVG_11_TestSuite.svg)

\* And again, svgcleaner 0.6.2 is kinda superb, but since it breaks most
of the files - it's pointless.

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.6.91/docs/images/ratio_chart_oxygen.svg)

#### Cleaning time

*Less is better.*

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.6.91/docs/images/performance_chart_W3C_SVG_11_TestSuite.svg)

![Alt text](https://cdn.rawgit.com/RazrFalcon/svgcleaner/v0.6.91/docs/images/performance_chart_oxygen.svg)

\* Even through that current results are impressive, svgcleaner is still very slow.

## Limitations

*svgcleaner* shouldn't change your file unless you say it to, but there are still
things that can't be preserved. So even if you disable all cleaning options there are still things
that will be changed:

- Original indent is not preserved.
- All colors will be formatted as #RRGGBB.
- DOCTYPE, CDATA will be processed and removed.
- CSS from the `style` element will be extracted and processes. The `style` element will be removed.
- `style` attribute will be split into attributes.
- `class` attribute will be processed and removed.
- Paths will be reformatted.
- `currentColor` and `inherit` attributes values will be resolved.
- Referenced elements will be moved to `defs` element, when possible.
- IRI and FuncIRI attributes that reference non-existing objects will be removed.

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

## Contributing and Issues

See [CONTRIBUTING.md](CONTRIBUTING.md) for details.

## Roadmap
V0.7
 - [x] Remove text-related attributes if there is no text.
 - [x] Ungroup groups.
 - [x] Remove duplicated defs:
   - [x] `linearGradient`
   - [x] `radialGradient`
   - [x] `feGaussianBlur`
 - [ ] Remove invisible elements.
   - [x] Invisible elements inside the `clipPath`.
   - [ ] Elements with invalid size.
   - [ ] other...
 - [ ] Remove needless attributes.
   - [x] Remove attributes which does not belong to this element.
     - [x] Basic shapes
     - [ ] other...
   - [x] Remove presentational attributes from elements inside `clipPath`.
   - [ ] Remove `stroke-linecap` if the path is closed.
   - [ ] other...
 - [ ] Group elements by style attributes.
   - [x] Move to group if all attributes are equal
   - [ ] Move to group if most attributes are equal
   - [ ] Create own groups.
 - [x] Remove `version` attribute.
 - [ ] Remove elements outside the viewbox.
 - [x] Merge gradients.
 - [ ] Apply transforms:
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
    - [x] remove ClosePath segments
    - [x] remove unneeded MoveTo segments
    - [ ] convert segments into shorter one
    - [ ] join line-based segments
 - [ ] Join sequential paths.
 - [x] Join presentational attributes.
 - [ ] Process `enable-background`.
 - [ ] Remove duplicated `stop` elements.
 - [ ] Resolve `use`.
 - [ ] Test more files.
 - [ ] Test files using all possible cleaning options combinations.
 - [ ] Performance optimizations.

V0.8
 - [ ] Convert units using specified DPI.
 - [ ] Numbers comparing using custom precision.
 - [ ] Simplify paths. Something like Inkscapes's 'Simplify', but lossless.
       Have no idea how to make it. Hints [here](https://pomax.github.io/bezierinfo/#bsplines).
 - [ ] Remove elements covered by other elements.
 - [ ] Test not only against webkit.
 - [ ] Unix pipes.

Note, that master branch is pretty stable and ready to use.

## License

*svgcleaner* is licensed under the [GPL-2.0](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html).
