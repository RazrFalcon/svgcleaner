Merge gradients
---------------

Many SVG editors split gradient implementation into two parts:
one element with ``stop`` children elements and one that linked to it.
It can be useful if we have a lot of gradients with equal stop's, but if we have only one - it
became pointless.

This option fixes it.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <defs>
..     <linearGradient id="lg1">
..       <stop offset="0"
..             stop-color="yellow"/>
..       <stop offset="1"
..             stop-color="green"/>
..     </linearGradient>
..     <linearGradient id="lg2"
..                     xlink:href="#lg1"/>
..   </defs>
..   <circle fill="url(#lg2)"
..           cx="50" cy="50" r="45"/>
.. </svg>
.. AFTER
.. <svg>
..   <defs>
..     <linearGradient id="lg2">
..       <stop offset="0"
..             stop-color="yellow"/>
..       <stop offset="1"
..             stop-color="green"/>
..     </linearGradient>
..   </defs>
..   <circle fill="url(#lg2)"
..           cx="50" cy="50" r="45"/>
.. </svg>
.. END
