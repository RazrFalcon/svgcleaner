Remove non-SVG attributes
-------------------------

We can remove any `non-SVG attributes <https://www.w3.org/TR/SVG/attindex.html>`_, since they are not rendered either way.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <circle fill="green" my-attribute="hi!"
..           cx="50" cy="50" r="45"/>
.. </svg>
.. AFTER
.. <svg>
..   <circle fill="green"
..           cx="50" cy="50" r="45"/>
.. </svg>
.. END
