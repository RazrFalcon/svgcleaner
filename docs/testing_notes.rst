.. contents::

Rendering errors
================

The main goal of the ``svgcleaner`` - is correctness. Cleaned files must be rendered exactly
the same as original. If objects in your image changed their colors, shapes or positions - it's
definitely an error. But things are not that easy in the SVG world.

Anti-aliasing
-------------

By default, all shapes in the SVG is rendered with anti-aliasing. But it doesn't mean that all
of the rendering application uses same techniques.
So even slightest changes in numbers rounding can lead to errors like this:

.. image:: images/compare_error.png

\* original, cleaned, diff

``svgcleaner`` does not treat such artifacts as errors.

Correctness according to the SVG spec
-------------------------------------

This is an even bigger problem since every rendering application supports their own SVG subset.
And even this subset can be implemented with errors. So cleaned image can became broken
only because rendering application will treat new data differently.

There are lots of examples (which is not categorized yet):

- These two paths are the same: ``M 10 -20 A 5.5 0.3 -4 1 1 0 -0.1``, ``M10-20A5.5.3-4 110-.1``.
  Sadly, most of the application doesn't support such notation, even throw it's valid by SVG spec.
- ``clipPath`` element can contain only specific elements, but some applications renders all of them.
  So when ``svgcleaner`` removes invalid elements - the result image will be rendered differently,
  even throw it's valid by SVG spec.
- And so on...

And again - such artifacts is not an error.

A bigger file
-------------

If ``svgcleaner`` produce bigger file than original - it will print an error.
During testing, when such error occurs, we just copy an original file to the destination,
which makes test to pass.

Testing method
==============

Testing algorithm looks like this:

1. Clean image with cleaning application.
#. Render original image using ``tools/svgrender`` [1]_.
#. Render cleaned image using ``tools/svgrender`` [1]_.
#. Compare images using imagemagick's ``compare``:

   ``compare -metric AE -fuzz 1% orig.png cleaned.png diff.png``
#. If AE value is bigger than 2% [2]_ of the image size - we have an error.

.. [1] ``svgrender`` is based on QtWebKit, which is the best way to render SVG from CLI.

.. [2] we use 2% threshold to filter anti-aliasing artifacts.
