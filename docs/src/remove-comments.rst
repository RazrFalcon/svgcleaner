Remove XML comments
-------------------

We can remove all XML comments from SVG document since they are not rendered either way.

**Note:** the `svgdom <https://github.com/RazrFalcon/libsvgdom>`_ library,
which is used to process SVG file in the ``svgcleaner``,
doesn't support comments inside attributes except ``style`` attribute,
so they will be removed anyway.

**Unsupported by:** librsvg <= 2.40.13 (it doesn't support comments inside attributes, so when we
remove them image will be rendered differently)

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
