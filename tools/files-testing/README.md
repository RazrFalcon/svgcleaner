# files-testing

Clean all files from the selected folder and check that all of them are cleaned correctly.

## Dependencies

- Linux
- imagemagic (we need 'compare')
- prebuild 'svgcleaner' itself, release build
- prebuild 'tools/svgrender'
- prebuild 'tools/err_view'

## Usage

```bash
stats \
      # path to folder where all temp files will be generated
      --workdir=/var/svg/svg/ \
      --input-data=/path_to_dir_with_svg_files \
      # (optional) path to config that contains list of ignored files
      # see ./data for examples
      --input-data-config=config.json \
      # each input-data must have own db
      --cache-db=/path_to_cache_file/stats.sqlite
```

## Files for testing
 - http://www.w3.org/Graphics/SVG/Test/20110816/archives/W3C_SVG_11_TestSuite.tar.gz
 - https://sourceforge.net/projects/openiconlibrary/
 - https://openclipart.org/downloads/
 - https://www.archlinux.org/packages/extra/any/oxygen-icons-svg/
 - https://github.com/KDE/breeze-icons
 - https://github.com/linuxdeepin/deepin-icon-theme
 - https://github.com/GreenRaccoon23/archdroid-icon-theme
 - https://github.com/elementary/icons
 - https://wiki.ubuntu.com/Artwork/Incoming/Karmic/Humanity_Icons?action=AttachFile&do=view&target=humanity_2.1.tar.gz
 - https://github.com/NitruxSA/ardis-icon-theme
 - https://www.archlinux.org/packages/community/any/faience-icon-theme/
 - https://github.com/GNOME/adwaita-icon-theme

## Convert svgz to svg

Useful for 'Oxygen icon theme'.

```bash
find . -name "*.svgz" | while read filename; do 7za e -so "$filename" > "${filename%.svgz}.svg"; done;
find . -name "*.svgz" -type f -delete
```
