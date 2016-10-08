# stats

Process folder with SVG files to gather statistics about `svgcleaner`.

**Note:** Linux only since it depends on imagemagic's `compare`.

## Usage

```bash
stats \
      # path to folder where all temp files will be generated
      --workdir=/tmp/svg/ \
      --input-data=/path_to_dir_with_svg_files \
      # one of: svgcleaner, svgcleaner-old, svgo
      --type=svgcleaner \
      --cleaner=/path_to_cleaner_exe \
      # 2% (0..100)
      --threshold=2
```

How to get results like in charts:
```bash
# svgcleaner 0.7.0
stats \
    --workdir=/tmp/svg/ \
    --input-data=/path_to_svg_dir \
    --cleaner=/path_to_svgcleaner \
    --type=svgcleaner \
    --threshold=2

# svgcleaner 0.6.2
stats \
    --workdir=/tmp/svg/ \
    --input-data=/path_to_svg_dir \
    --cleaner=/path_to_old_svgcleaner \
    --type=svgcleaner-old \
    --threshold=2

# svgo
stats \
    --workdir=/tmp/svg/ \
    --input-data=/path_to_svg_dir \
    --cleaner=/path_to_svgo \
    --type=svgo \
    --threshold=2
```
