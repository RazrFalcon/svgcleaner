Convert path segments into shorter ones
---------------------------------------

Some segments can be represented using different segment types
keeping a resulting shape exactly the same.
We only use conversions that make path notation shorter.

Currently supported conversions are:

- LineTo -> HorizontalLineTo
- LineTo -> VerticalLineTo
- CurveTo -> HorizontalLineTo
- CurveTo -> VerticalLineTo
- CurveTo -> LineTo
- CurveTo -> SmoothCurveTo

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <path fill="none" stroke="red"
..         stroke-width="2"
..         d="M 10 15 C 10 15 72.5 10 72.5 55
..            C 72.5 100 135 100 135 55 L 10 55"/>
.. </svg>
.. AFTER
.. <svg>
..   <path fill="none" stroke="red"
..         stroke-width="2"
..         d="M 10 15 S 72.5 10 72.5 55
..            S 135 100 135 55 H 10"/>
.. </svg>
.. END

