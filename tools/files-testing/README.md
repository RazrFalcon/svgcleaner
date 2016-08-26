Files for testing:
 - http://www.w3.org/Graphics/SVG/Test/20110816/archives/W3C_SVG_11_TestSuite.tar.gz
 - https://sourceforge.net/projects/openiconlibrary/
 - https://openclipart.org/downloads/
 - https://www.archlinux.org/packages/extra/any/breeze-icons/
 - https://www.archlinux.org/packages/extra/any/oxygen-icons-svg/
 - https://github.com/linuxdeepin/deepin-icon-theme
 - https://github.com/GreenRaccoon23/archdroid-icon-theme
 - https://github.com/elementary/icons

Convert svgz to svg:
```
find . -name "*.svgz" | while read filename; do 7za e -so "$filename" > "${filename%.svgz}.svg"; done;
find . -name "*.svgz" -type f -delete
```

### Notes about SVG files testing

Testing cleaned SVG files for errors are basicaly the same pain, as cleaning them.

First of all, every SVG render renders differently. Very differently.
The best so far is WebKit (the best from CLI for sure).
If we forget, for a second, about supported SVG subset, we will left with different
antialiasing, scaling, blur methods etc. Even slightest change in item position can lead
to it's edges colors difference, so we need to use color fuzzing to skip such kind of 'errors'.
