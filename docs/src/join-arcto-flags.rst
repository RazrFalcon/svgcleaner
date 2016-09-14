Join ArcTo flags
----------------

Elliptical arc curve segment has flags parameters, which can have values of ``0`` or ``1``.
Since we have fixed-width values, we can skip spaces between them.

**Note:** Sadly, but most of the viewers doesn't support such notation, even throw it's valid by SVG spec.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <path fill="green" stroke="red"
..         stroke-width="2"
..         d="M 30 60
..            a 25 25 -30 1 1 50 -20"/>
.. </svg>
.. AFTER
.. <svg>
..   <path fill="green" stroke="red"
..         stroke-width="2"
..         d="M 30 60
..            a 25 25 -30 1150 -20"/>
.. </svg>
.. END
