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

## Can you add an option to specify numeric precision?

No. It was a difficult decision, but it's hard to implement numeric rounding in a lossless way.

However, previous versions of the *svgcleaner* did have this option, as well as other SVG cleaning applications.
And it's actually pretty good for size reduction. But usually, it's just a straightforward
number rounding without a context, which is very dangerous in some cases.

**Here's one example:**

Let's take a very small rect:
```svg
<rect height="0.0001" width="0.0001"/>
```
It's obvious that it will not be rendered. With a small precision, it will have a size of zero and
can even be removed completely. That's how non-contextual rounding works.

But what if it actually looks like this?:
```svg
<rect height="0.0001" width="0.0001" transform="scale(100000)"/>
```

Well, in this case, we just broke an image. And there are lots of examples like this.

This option will be added only in the case that I find a way to prove every image will be unchanged.

## Why is it so fast?

It's not. *svgcleaner* could be many times faster, but it's not a current priority.
