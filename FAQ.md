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

No. ``svgcleaner`` intended as a lossless cleaner. It doesn't remove anything that
can impact rendering.

## But you remove title, desc, etc.

Yes. But those elements doesn't impact rendering.

## Can you add an option to specify a numeric precision?

No. It was a difficult decision, but it's hard to implement a numeric rounding in a lossless way.

Yes, previous versions of the svgcleaner had this option as well as other SVG cleaning applications.
And it's actually pretty good for size reducing. But usually, it's just a straightforward
number rounding without a context, which is very dangerous in some cases.

Here is example:

Let's take a very small rect:
```svg
<rect height="0.0001" width="0.0001"/>
```
It's obvious that it will not be rendered. With a small precision, it will have a size of zero and
even can be removed completely. That's how non-contextual rounding works.

But what if it actually looks like this:
```svg
<rect height="0.0001" width="0.0001" transform="scale(100000)"/>
```

Well, in this case, we just broke an image. And there are lots of examples like this.

This option will be added only in case if I found a way to prove that image will be unchanged.

## Why it's fast?

It's not. svgcleaner can be times faster, but it's not a current priority.
