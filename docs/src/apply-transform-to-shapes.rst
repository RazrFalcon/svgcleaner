Apply transformations to shapes
-------------------------------

Transformations that contain only translate, rotate and/or proportional scale parts
can be applied to some shapes.

This option will apply transformations to: ``rect``, ``circle``, ``ellipse`` and ``line``.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <circle fill="green" stroke-width='0'
..           transform="translate(10 10) scale(2)"
..           cx="20" cy="20" r="22"/>
.. </svg>
.. AFTER
.. <svg>
..   <circle fill="green" stroke-width='0'
..           cx="50" cy="50" r="44"/>
.. </svg>
.. END
