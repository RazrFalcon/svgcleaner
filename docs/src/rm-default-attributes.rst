Remove presentation attributes with default values
--------------------------------------------------

We can remove presentation attributes with default values if they not covered by parent elements.

In example bellow we have ``circle`` element with ``fill`` and ``stroke`` attributes, which have default values. We can't remove ``fill`` from ``circle``, because than rect will be filled red, but ``stroke`` can be easily removed.

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
