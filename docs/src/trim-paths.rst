Use compact notation for paths
------------------------------

By SVG spec we are allowed to remove some symbols from path notation without breaking parsing.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <path fill="green" stroke="red"
..         stroke-width="2"
..         d="M 30 60 a 25 25 -30 1 1 50,-20
..            l 0.5 0.5 l 30 60 z"/>
.. </svg>
.. AFTER
.. <svg>
..   <path fill="green" stroke="red"
..         stroke-width="2"
..         d="M30 60a25 25-30 1 1 50-20l.5.5l30 60z"/>
.. </svg>
.. END
