Remove unreferenced ``id`` attributes
-------------------------------------

We can remove ``id`` attribute from an element if this ``id`` doesn't use in any IRI/FuncIRI.

**Note:** since ``svgcleaner`` works only with static/local SVG data and does not support
SVG scripting via ``script`` element, we can only assume that ``id`` is not used.

.. GEN_TABLE
.. BEFORE
.. <svg id="svg1">
..   <circle id="circle1" fill="green"
..           cx="50" cy="50" r="50"/>
..   <circle id="circle2" fill="#023373"
..           cx="100" cy="50" r="50"/>
..   <use id="use1" x="100" xlink:href="#circle1"/>
.. </svg>
.. AFTER
.. <svg>
..   <circle id="circle1" fill="green"
..           cx="50" cy="50" r="50"/>
..   <circle fill="#023373"
..           cx="100" cy="50" r="50"/>
..   <use x="100" xlink:href="#circle1"/>
.. </svg>
.. END
