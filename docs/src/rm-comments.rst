Remove XML comments
-------------------

We can remove all XML comments from SVG document, since they are not rendered either way.

**Note:** ``svgdom`` library, which is used to process SVG file in SVG Cleaner,
doesn't support comments inside attributes, so they will be removed anyway.

.. GEN_TABLE
.. BEFORE
.. <!-- Comment -->
.. <svg>
..   <!-- Comment -->
..   <circle style="/* comment */stroke:black"
..           fill="green" cx="50" cy="50" r="45"/>
.. </svg>
.. AFTER
.. <svg>
..   <circle style="stroke:black" fill="green"
..           cx="50" cy="50" r="45"/>
.. </svg>
.. END
