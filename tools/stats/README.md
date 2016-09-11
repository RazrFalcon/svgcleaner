# stats

Process folder with SVG files to gather statistics about `svgcleaner`.

**Note:** Linux only since it depends on imagemagic's `compare`.

## Usage

```bash
stats \
      # path to folder where all temp files will be generated
      --workdir=/var/svg/svg/ \
      --input-data=/path_to_dir_with_svg_files \
      # one of: svgcleaner, svgcleaner-old, svgo
      --type=svgcleaner \
      --cleaner=/path_to_cleaner_exe \
      # ../svgcleaner/tools/svgrender/
      # you can set --skip-errors-check instead
      --render=/path_to/svgrender \
      # each input-data must have own db
      --cache-db=/path_to_cache_file/stats.sqlite \
      # 2% (0..100)
      --threshold=2
```
