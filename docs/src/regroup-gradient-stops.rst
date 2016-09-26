Regroup gradient ``stop`` elements
----------------------------------

If two or more gradients have equal ``stop`` elements - we can move this elements
into a new ``linearGradient`` and link gradients to this new gradient.

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
..   </defs>
..   <circle fill="url(#lg1)"
..           cx="50" cy="50" r="45"/>
..   <circle fill="url(#lg2)"
..           cx="100" cy="50" r="45"/>
.. </svg>
.. AFTER
.. <svg>
..   <defs>
..     <linearGradient id="lg3">
..       <stop offset="0"
..             stop-color="yellow"/>
..       <stop offset="1"
..             stop-color="green"/>
..     </linearGradient>
..     <linearGradient id="lg1" xlink:href="#lg3"/>
..     <linearGradient id="lg2" xlink:href="#lg3"/>
..   </defs>
..   <circle fill="url(#lg1)"
..           cx="50" cy="50" r="45"/>
..   <circle fill="url(#lg2)"
..           cx="100" cy="50" r="45"/>
.. </svg>
.. END
