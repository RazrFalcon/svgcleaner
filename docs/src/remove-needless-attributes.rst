Remove attributes that doesn't belong to this element
-----------------------------------------------------

Remove attributes that doesn't belong to current element and have no effect on rendering.

Unlike other cleaning options for attributes, this does not change attributes that can
be used during rendering.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <clipPath id="cp1">
..     <rect fill="red" stroke="red"
..           stroke-width="50" width="75"
..           height="75"/>
..   </clipPath>
..   <circle fill="green" d="M 10 20 L 30 40"
..           clip-path="url(#cp1)"
..           cx="50" cy="50" r="45"/>
.. </svg>
.. AFTER
.. <svg>
..   <clipPath id="cp1">
..     <rect width="75" height="75"/>
..   </clipPath>
..   <circle fill="green" clip-path="url(#cp1)"
..           cx="50" cy="50" r="45"/>
.. </svg>
.. END
