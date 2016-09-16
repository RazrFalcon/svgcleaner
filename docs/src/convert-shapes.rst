Convert basic shapes into paths
-------------------------------

All basic shapes can be represented as ``path``.

``circle``, ``ellipse`` and rounded ``rect`` are ignored, because their path representation will
always be bigger than original.

**Note:** shapes may render a bit differently depending on your user agent.
You can use `shape-rendering <https://www.w3.org/TR/SVG/painting.html#ShapeRenderingProperty>`_
attribute to tweak it.

.. GEN_TABLE
.. BEFORE
.. <svg id="svg1">
..   <rect id="rect1" x="10" y="10"
..          width="80" height="80"/>
..   <line id="line1" stroke="red" x1="10"
..         y1="90" x2="90" y2="10"/>
..   <polyline id="polyline1" stroke="blue"
..             fill="none"
..             points="10 10 30 10 30 30
..                     50 30 50 50"/>
..   <polygon id="polygon1" stroke="green"
..               fill="none"
..               points="10 10 10 30 30 30
..                       30 50 50 50"/>
.. </svg>
.. AFTER
.. <svg>
..   <path id="rect1"
..         d="M 10 10 H 90 V 90 H 10 Z"/>
..   <path id="line1" stroke="red"
..         d="M 10 90 L 90 10"/>
..   <path id="polyline1" stroke="blue"
..         fill="none"
..         d="M 10 10 30 10 30 30
..            50 30 50 50"/>
..   <path id="polygon1" stroke="green"
..         fill="none"
..         d="M 10 10 10 30
..            30 30 30 50 50 50 Z"/>
.. </svg>
.. END
