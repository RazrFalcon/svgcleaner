Convert path segments into relative ones
----------------------------------------

Since segments of the path data can be set in absolute and relative coordinates -
we can convert all of them into relative one, which is generally shorter.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <path d="M 750 150 L 800 200 L 850 150
..            L 850 250 L 850 350 L 800 300
..            L 750 350 L 750 250 Z"
..         transform="scale(0.1)"
..         fill="green"/>
.. </svg>
.. AFTER
.. <svg>
..   <path d="m 750 150 l 50 50 l 50 -50
..            l 0 100 l 0 100 l -50 -50
..            l -50 50 l 0 -100 z"
..         transform="scale(0.1)"
..         fill="green"/>
.. </svg>
.. END
