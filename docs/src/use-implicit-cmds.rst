Use implicit LineTo commands
----------------------------

By SVG spec: 'if a moveto is followed by multiple pairs of coordinates, the subsequent pairs
are treated as implicit lineto commands'.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <path fill="green" stroke="red"
..         stroke-width="2"
..         d="M 10 10 L 50 50 L 120 50"/>
.. </svg>
.. AFTER
.. <svg>
..   <path fill="green" stroke="red"
..         stroke-width="2"
..         d="M 10 10 50 50 120 50"/>
.. </svg>
.. END
