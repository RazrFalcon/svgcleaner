Apply transformations to paths
------------------------------

Transformations that contain only translate, rotate and/or proportional scale parts
can be applied to some paths.

This usually creates bigger files, so it's disabled by default.
But it some cases it can be useful.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <path stroke="red"
..         transform="translate(10 20)"
..         d="M 10 0 L 30 40"/>
.. </svg>
.. AFTER
.. <svg>
..   <path stroke="red"
..         d="M 20 20 L 40 60"/>
.. </svg>
.. END
