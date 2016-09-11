# files-testing

Clean all files in selected folder and check that all of the are cleaned correctly.

**Note:** Linux only since it depends on imagemagic's `compare`.

## Usage

```bash
stats \
      # path to folder where all temp files will be generated
      --workdir=/var/svg/svg/ \
      --input-data=/path_to_dir_with_svg_files \
      --svgcleaner=/path_to_cleaner_exe \
      # ../svgcleaner/tools/svgrender/
      --render=/path_to/svgrender \
      # path to config that contains list of ignored files
      # see ./data for examples
      --input-data-config=config.toml \
      # each input-data must have own db
      --cache-db=/path_to_cache_file/stats.sqlite \
      # continue testing from the last position
      --continue
```

## Files for testing
 - http://www.w3.org/Graphics/SVG/Test/20110816/archives/W3C_SVG_11_TestSuite.tar.gz
 - https://sourceforge.net/projects/openiconlibrary/
 - https://openclipart.org/downloads/
 - https://www.archlinux.org/packages/extra/any/breeze-icons/
 - https://www.archlinux.org/packages/extra/any/oxygen-icons-svg/
 - https://github.com/linuxdeepin/deepin-icon-theme
 - https://github.com/GreenRaccoon23/archdroid-icon-theme
 - https://github.com/elementary/icons

## Convert svgz to svg

Useful for 'Oxygen icon theme'.

```
find . -name "*.svgz" | while read filename; do 7za e -so "$filename" > "${filename%.svgz}.svg"; done;
find . -name "*.svgz" -type f -delete
```

## Notes about SVG files testing

Testing cleaned SVG files for errors are basically the same pain, as cleaning them.

First of all, every SVG render renders differently. Very differently.
The best so far is WebKit (the best from CLI for sure).
If we forget, for a second, about supported SVG subset, we will left with different
anti-aliasing, scaling, blur methods etc. Even slightest change in item position can lead
to it's edges colors difference, so we need to use color fuzzing to skip such kind of 'errors'.
