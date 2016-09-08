Remove attributes with default values
-------------------------------------

We can remove attributes with default values if they are not covered by the parent elements.
Some attributes do not support an inheritance, so we can remove them
without checking a parent elements.

In the example below we have a ``circle`` element with a ``fill`` and a ``stroke`` attributes,
which have default values. We can't remove a ``fill`` from a ``circle``, because than the rect
will be filled with a red, but a ``stroke`` can be easily removed.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <g fill="red">
..     <circle fill="black" stroke="none"
..             cx="50" cy="50" r="45"/>
..   </g>
.. </svg>
.. AFTER
.. <svg>
..   <g fill="red">
..     <circle fill="black" cx="50"
..             cy="50" r="45"/>
..   </g>
.. </svg>
.. END
