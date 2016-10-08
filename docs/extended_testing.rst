Extended testing
================

Here we testing ``svgcleaner`` using ``tools/stats``.

.. contents::

W3C SVG 1.1 TestSuite
---------------------

::

  Files count: 525
  Files cleaned with serious errors: 1
  Files cleaned with any errors: 88
  Unchanged files: 228
  Size after/before: 2244463/3310597
  Cleaning ratio: 32.20%
  Cleaning time: 734.8ms total, 1.3996ms avg

Broken images:

- ``struct-frag-05-t.svg`` - custom namespaces are not supported in ``libsvgdom``.

Oxygen icons theme 4.12
-----------------------

::

  Files count: 4941
  Files cleaned with serious errors: 28
  Files cleaned with any errors: 909
  Unchanged files: 2
  Size after/before: 506672479/1277145470
  Cleaning ratio: 60.33%
  Cleaning time: 36661.3ms total, 7.4198ms avg

Broken images:

- ``devices/phone.svg`` - magic bug. File is actually broken, but when I'm trying to edit it in
  Inkscape, to find a problem, - Inkscape crashes.
- ``devices/small/16x16/network-wireless.svg`` - blur artifacts because of numeric rounding.
- ``actions/small/48x48/taxes-finances.svg`` - broken ``pattern``. *svgcleaner* actually fixes it.
- Other images are broken because of QtWebKit bugs.

So svgcleaner has broken only one file.

Breeze icons theme
------------------

Version: https://github.com/KDE/breeze-icons/tree/4dbb0b3e7079028bde5a58a2d82678a1697776f0

::

  Files count: 8282
  Files cleaned with serious errors: 4
  Files cleaned with any errors: 150
  Unchanged files: 30
  Size after/before: 11637706/22776691
  Cleaning ratio: 48.91%
  Cleaning time: 11507.5ms total, 1.3895ms avg

Broken images:

- All four images are rendered incorrectly because of QtWebKit bugs.

Ardis icons theme
-----------------

::

  Files count: 1663
  Files cleaned with serious errors: 0
  Files cleaned with any errors: 21
  Unchanged files: 0
  Size after/before: 5485887/12396061
  Cleaning ratio: 55.74%
  Cleaning time: 2611.0ms total, 1.5700ms avg

Humanity icons theme 2.1
------------------------

::

  Files count: 577
  Files cleaned with serious errors: 5
  Files cleaned with any errors: 357
  Unchanged files: 0
  Size after/before: 3231811/8979700
  Cleaning ratio: 64.01%
  Cleaning time: 1102.9ms total, 1.9114ms avg

Broken images:

- All five images are rendered incorrectly because of QtWebKit bugs.

Open Icon Library
-----------------

**Note:** we must remove ``symbols`` folder, because it contains mostly broken files.

::

  Files count: 2574
  Files cleaned with serious errors: 11
  Files cleaned with any errors: 1032
  Unchanged files: 32
  Size after/before: 72819002/217532687
  Cleaning ratio: 66.53%
  Cleaning time: 9951.1ms total, 3.8660ms avg

Broken images:

- ``phone-4.svg`` - magic bug. File is actually broken, but when I'm trying to edit it in
  Inkscape, to find a problem, - Inkscape crashes.
- ``network-wireless-4.svg`` - blur artifacts because of numeric rounding.
- Other images are broken because of QtWebKit bugs.

Notes
-----

- *Files cleaned with any errors* - is showing that file has at least one changed pixel.
- *Unchanged files* - amount of files that was skipped during testing.
  They was probably invalid, unsupported or bigger than original.
- Links for all icon sets can be found in ``tools/files_testing/README.md``.