Remove invisible elements
-------------------------

The collection of algorithms that detects invisible elements and removes them.

**Unsupported by:** QtSvg <= 5.7

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <linearGradient id="lg1"/>
..   <clipPath id="cp1"/>
..   <circle fill="green"
..           cx="50" cy="50" r="45"/>
..   <circle fill="green" clip-path="url(#cp1)"
..           stroke="url(#lg1)"
..           cx="100" cy="50" r="45"/>
.. </svg>
.. AFTER
.. <svg>
..   <circle fill="green"
..           cx="50" cy="50" r="45"/>
.. </svg>
.. END
