Apply transformations to gradients
----------------------------------

Transformations that contain only translate, rotate and/or proportional scale parts
can be applied to some gradients.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <linearGradient id="lg1" x1="40" y1="30"
..           x2="90" y2="30"
..           gradientTransform="translate(10 20)"
..           gradientUnits="userSpaceOnUse">
..     <stop offset="0"
..           stop-color="yellow"/>
..     <stop offset="1"
..           stop-color="green"/>
..   </linearGradient>
..   <circle fill="url(#lg1)"
..           cx="50" cy="50" r="45"/>
.. </svg>
.. AFTER
.. <svg>
..   <linearGradient id="lg1" x1="50" y1="50"
..           x2="100" y2="50"
..           gradientUnits="userSpaceOnUse">
..     <stop offset="0"
..           stop-color="yellow"/>
..     <stop offset="1"
..           stop-color="green"/>
..   </linearGradient>
..   <circle fill="url(#lg1)"
..           cx="50" cy="50" r="45"/>
.. </svg>
.. END
