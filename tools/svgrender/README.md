# svgrender

Render SVG files using QtWebKit.

A bit changed [demo](https://blog.qt.io/blog/2008/08/06/webkit-based-svg-rasterizer/) from the Qt dojo.

## Details

- The canvas is always rectangle.
- The image rendered in the center of the canvas.
- Image will be scaled to the selected canvas size.

## Dependencies

Qt 5.5 (or any newer but with QtWebKit module, QtWebEngine is not supported).

## Build

```bash
qmake
make
```
