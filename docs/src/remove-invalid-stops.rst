Remove invalid ``stop`` elements
--------------------------------

We can remove duplicated ``stop`` elements inside gradients.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <defs>
..     <linearGradient id="lg1">
..        <stop offset="-1" stop-color="yellow"/>
..        <stop offset="0" stop-color="yellow"/>
..        <stop offset="0.5" stop-color="green"/>
..        <stop offset="0.5" stop-color="green"/>
..        <stop offset="1" stop-color="yellow"/>
..        <stop offset="10" stop-color="yellow"/>
..     </linearGradient>
..   </defs>
..   <circle fill="url(#lg1)"
..           cx="50" cy="50" r="45"/>
.. </svg>
.. AFTER
.. <svg>
..   <defs>
..     <linearGradient id="lg1">
..        <stop offset="0" stop-color="yellow"/>
..        <stop offset="0.5" stop-color="green"/>
..        <stop offset="1" stop-color="yellow"/>
..     </linearGradient>
..   </defs>
..   <circle fill="url(#lg1)"
..           cx="50" cy="50" r="45"/>
.. </svg>
.. END
