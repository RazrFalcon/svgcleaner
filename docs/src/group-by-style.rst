Group elements by equal styles
------------------------------

If a continuous range of elements contains equal, inheritable attributes - we can
group such elements and move this attributes to a new or an existing parent group.

**Note:** this option is mostly poinless when XML indent is enabled,
so you should use it with *Sets XML nodes indent*/``--indent`` option equal to ``-1`` or ``0``.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <circle fill="green" r="45"
..           cx="50" cy="50"/>
..   <circle fill="green" r="45"
..           cx="100" cy="50"/>
..   <circle fill="green" r="45"
..           cx="150" cy="50"/>
.. </svg>
.. AFTER
.. <svg>
..   <g fill="green">
..     <circle r="45"
..             cx="50" cy="50"/>
..     <circle r="45"
..             cx="100" cy="50"/>
..     <circle r="45"
..             cx="150" cy="50"/>
..   </g>
.. </svg>
.. END
