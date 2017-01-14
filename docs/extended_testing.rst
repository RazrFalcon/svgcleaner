Extended testing
================

Here we testing ``svgcleaner`` using ``tools/stats``.

Currently, we are testing against 20779 files.

.. contents::

W3C SVG 1.1 TestSuite
---------------------

::

  Files count: 525
  Files cleaned with serious errors: 1
  Files cleaned with any errors: 88
  Unchanged files: 228
  Size after/before: 2226642/3310597
  Cleaning ratio: 32.74%
  Cleaning time: 749.6ms total, 1.4279ms avg

Broken images:

- ``struct-frag-05-t.svg`` - custom namespaces are not supported in ``libsvgdom``.

Oxygen icons theme 4.12
-----------------------

::

  Files count: 4941
  Files cleaned with serious errors: 26
  Files cleaned with any errors: 907
  Unchanged files: 2
  Size after/before: 497120597/1277145470
  Cleaning ratio: 61.08%
  Cleaning time: 38757.0ms total, 7.8440ms avg

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
  Unchanged files: 21
  Size after/before: 11477419/22776691
  Cleaning ratio: 49.61%
  Cleaning time: 12294.2ms total, 1.4844ms avg

Broken images:

- All four images are rendered incorrectly because of QtWebKit bugs.

Ardis icons theme
-----------------

::

  Files count: 1663
  Files cleaned with serious errors: 0
  Files cleaned with any errors: 21
  Unchanged files: 0
  Size after/before: 5468383/12396061
  Cleaning ratio: 55.89%
  Cleaning time: 2805.4ms total, 1.6870ms avg

Humanity icons theme 2.1
------------------------

::

  Files count: 577
  Files cleaned with serious errors: 5
  Files cleaned with any errors: 358
  Unchanged files: 0
  Size after/before: 3183734/8979700
  Cleaning ratio: 64.55%
  Cleaning time: 1217.9ms total, 2.1107ms avg

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
  Size after/before: 71748144/217532687
  Cleaning ratio: 67.02%
  Cleaning time: 11156.2ms total, 4.3342ms avg

Broken images:

- ``phone-4.svg`` - magic bug. File is actually broken, but when I'm trying to edit it in
  Inkscape, to find a problem, - Inkscape crashes.
- ``network-wireless-4.svg`` - blur artifacts because of numeric rounding.
- Other images are broken because of QtWebKit bugs.

Elementary icons theme
----------------------

::

  Files count: 1764
  Files cleaned with serious errors: 0
  Files cleaned with any errors: 252
  Unchanged files: 0
  Size after/before: 7361706/18585716
  Cleaning ratio: 60.39%
  Cleaning time: 3388.8ms total, 1.9211ms avg

Adwaita icons theme
-------------------

::

  Files count: 453
  Files cleaned with serious errors: 0
  Files cleaned with any errors: 26
  Unchanged files: 0
  Size after/before: 378084/2320391
  Cleaning ratio: 83.71%
  Cleaning time: 718.0ms total, 1.5850ms avg

Faience icon theme 0.5.1
------------------------

::

  Files count: 629
  Files cleaned with serious errors: 0
  Files cleaned with any errors: 16
  Unchanged files: 0
  Size after/before: 11563644/23440641
  Cleaning ratio: 50.67%
  Cleaning time: 1783.7ms total, 2.8357ms avg

Notes
-----

- *Files cleaned with any errors* - is showing that file has at least one changed pixel.
- *Unchanged files* - amount of files that was skipped during testing.
  They was probably invalid, unsupported or bigger than original.
- Links for all icon sets can be found in ``tools/files_testing/README.md``.
