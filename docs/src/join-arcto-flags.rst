Join ArcTo flags
----------------

Elliptical arc curve segment has flags parameters, which can have values of ``0`` or ``1``.
Since we have fixed-width values, we can skip spaces between them.

**Unsupported by:** Inkscape <= 0.91 r13725, QtSvg <= 5.7, librsvg <= 2.40.13

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
