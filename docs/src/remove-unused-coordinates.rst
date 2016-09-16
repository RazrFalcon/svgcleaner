Remove unused coordinate attributes
-----------------------------------

Many of coordinate attributes can be calculated using their neighbor attributes,
so there is no need to keep them.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <rect x="10" y="10" width="80"
..         height="80" fill="green"
..         rx="10" ry="10"/>
.. </svg>
.. AFTER
.. <svg>
..   <rect x="10" y="10" width="80"
..         height="80" fill="green"
..         rx="10"/>
.. </svg>
.. END
