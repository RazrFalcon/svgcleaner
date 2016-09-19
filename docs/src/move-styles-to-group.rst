Move presentational attributes to the parent group
--------------------------------------------------

If all children of the group element have the same presentation attribute - we can move this
attribute to the group and remove it from children.

This method does not create new groups.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <g>
..     <circle fill="green"
..             cx="50" cy="50" r="45"/>
..     <circle fill="green"
..             cx="100" cy="50" r="45"/>
..     <circle fill="green"
..             cx="150" cy="50" r="45"/>
..   </g>
.. </svg>
.. AFTER
.. <svg>
..   <g fill="green">
..     <circle cx="50" cy="50"
..             r="45"/>
..     <circle cx="100" cy="50"
..             r="45"/>
..     <circle cx="150" cy="50"
..             r="45"/>
..   </g>
.. </svg>
.. END
