Remove duplicated ``linearGradient`` elements
---------------------------------------------

An SVG can contain a lot of ``linearGradient`` elements, which may render exactly the same.
So we can remove duplicates and update links in elements, that uses them.

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
..     <linearGradient id="lg2">
..       <stop offset="0"
..             stop-color="yellow"/>
..       <stop offset="1"
..             stop-color="green"/>
..     </linearGradient>
..     <linearGradient id="lg3"
..                     xlink:href="#lg2"/>
..   </defs>
..   <circle fill="url(#lg1)"
..           cx="50" cy="50" r="45"/>
..   <circle fill="url(#lg2)"
..           cx="100" cy="50" r="45"/>
..   <circle fill="url(#lg3)"
..           cx="150" cy="50" r="45"/>
.. </svg>
.. AFTER
.. <svg>
..   <defs>
..     <linearGradient id="lg1">
..       <stop offset="0"
..             stop-color="yellow"/>
..       <stop offset="1"
..             stop-color="green"/>
..     </linearGradient>
..   </defs>
..   <circle fill="url(#lg1)"
..           cx="50" cy="50" r="45"/>
..   <circle fill="url(#lg1)"
..           cx="100" cy="50" r="45"/>
..   <circle fill="url(#lg1)"
..           cx="150" cy="50" r="45"/>
.. </svg>
.. END
