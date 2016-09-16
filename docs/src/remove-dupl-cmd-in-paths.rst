Remove subsequent segments command from paths
---------------------------------------------

If path segment has the same type as previous - we can skip command specifier.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <path d="M 10 10 L 90 10 L 90 90
..            L 10 90 L 10 10 z"
..         fill="none" stroke="red"
..         stroke-width="2"/>
.. </svg>
.. AFTER
.. <svg>
..   <path d="M 10 10 L 90 10 90 90
..            10 90 10 10 z"
..         fill="none" stroke="red"
..         stroke-width="2"/>
.. </svg>
.. END
