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
..     <circle fill="green" cx="50" cy="50" r="45"/>
..     <g>
..       <circle fill="#023373" cx="100" cy="50" r="45"/>
..     </g>
..   </g>
.. </svg>
.. AFTER
.. <svg>
..   <circle fill="green" cx="50" cy="50" r="45"/>
..   <circle fill="#023373" cx="100" cy="50" r="45"/>
.. </svg>
.. END
