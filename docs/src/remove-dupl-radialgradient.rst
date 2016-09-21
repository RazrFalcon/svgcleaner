Remove duplicated ``radialGradient`` elements
---------------------------------------------

An SVG can contain a lot of ``radialGradient`` elements, which may render exactly the same.
So we can remove duplicates and update links in elements, that uses them.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <defs>
..     <radialGradient id="rg1">
..       <stop offset="0"
..             stop-color="yellow"/>
..       <stop offset="1"
..             stop-color="green"/>
..     </radialGradient>
..     <linearGradient id="lg1">
..       <stop offset="0"
..             stop-color="yellow"/>
..       <stop offset="1"
..             stop-color="green"/>
..     </linearGradient>
..     <radialGradient id="rg2"
..                     xlink:href="#lg1"/>
..   </defs>
..   <circle fill="url(#rg1)"
..           cx="50" cy="50" r="45"/>
..   <circle fill="url(#rg2)"
..           cx="100" cy="50" r="45"/>
.. </svg>
.. AFTER
.. <svg>
..   <defs>
..     <radialGradient id="rg1">
..       <stop offset="0"
..             stop-color="yellow"/>
..       <stop offset="1"
..             stop-color="green"/>
..     </radialGradient>
..   </defs>
..   <circle fill="url(#rg1)"
..           cx="50" cy="50" r="45"/>
..   <circle fill="url(#rg1)"
..           cx="100" cy="50" r="45"/>
.. </svg>
.. END
