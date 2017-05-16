## Could svgcleaner break my image?

Well, that's a tricky question. Everything depends on what you call a broken image.
*svgcleaner* will reduce the file size according to the SVG spec. So, unless there is a bug
in the implementation, your file should be rendered the same. Problems may arise when you
use an SVG rendering application that doesn't support the SVG spec correctly... Then you might get
a "broken" image. But it will be broken because the rendering application doesn't support correct
optimization as performed by *svgcleaner*, not because *svgcleaner* broke your image.

Anyway, if you are using a modern browser - everything should be fine.

See [Testing notes](docs/testing_notes.rst) for details.

## Can you add an option to remove binary images from the SVG?

No. *svgcleaner* is intended as a lossless cleaner. It doesn't remove anything that
can impact rendering.

## But you remove title, desc, etc...

Yes. But those elements don't impact rendering.

## Why is it so fast?

It's not. *svgcleaner* could be many times faster, but it's not a current priority.
