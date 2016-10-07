Remove non-SVG elements
-----------------------

We can remove any XML elements with `non-SVG tag names <https://www.w3.org/TR/SVG/eltindex.html>`_,
since they are not rendered either way.

**Note:** elements from the SVG Tiny 1.2 and SVG 2.0 (unreleased) will also be removed.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <myelement/>
..   <circle fill="green" cx="50" cy="50" r="45"/>
.. </svg>
.. AFTER
.. <svg>
..   <circle fill="green" cx="50" cy="50" r="45"/>
.. </svg>
.. END
