Ungroup groups
--------------

Groups, aka ``g`` element, is one of the main SVG structure blocks,
but in a lot of cases they do not impact rendering at all.

Groups are useless:
 - if the group is empty
 - if the group has only one children
 - if the group doesn't have any important attributes

Then we can ungroup it and remove.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <g>
..     <circle fill="green" r="45"
..             cx="50" cy="50"/>
..     <g>
..       <circle fill="#023373" r="45"
..               cx="100" cy="50"/>
..     </g>
..   </g>
.. </svg>
.. AFTER
.. <svg>
..   <circle fill="green" r="45"
..           cx="50" cy="50"/>
..   <circle fill="#023373" r="45"
..           cx="100" cy="50"/>
.. </svg>
.. END
