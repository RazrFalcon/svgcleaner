## Can svgcleaner break my image?

Well, it's a tricky question. Everything depends on what you call a broken image.
svgcleaner will reduce the file size according to the SVG spec. So, unless there is a bug
in the implementation, - your file should be rendered the same. All problems begin when you
use an SVG rendering application that doesn't support an SVG spec correctly. Then you can get
a "broken" image. But it will be broken because the rendering application doesn't support
optimization is done by svgcleaner and not because svgcleaner broke your image.

Anyway, if you are using a modern browser - everything should be fine.

See [Testing notes](docs/testing_notes.rst) for details.

## Can you add an option to remove binary images from the SVG?

No. svgcleaner intended as a lossless cleaner. It doesn't remove anything that can impact rendering.

## But you remove title, desc, etc.

Yes. But those elements doesn't impact rendering.

## Can you add an option to specify a numeric precision?

No. Any manipulations with a numeric precision will lead to rendering errors.

## Why it's fast?

It's not. svgcleaner can be times faster, but it's not a current priority.
