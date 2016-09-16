Remove unused path segments
---------------------------

The collection of algorithms that removes unneeded segments from paths.

**NOTE:** can be used only with ``--paths-to-relative``.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <path stroke="red"
..         d="M 10 10 L 10 50 L 10 10 M 50 50 L 50 50"/>
.. </svg>
.. AFTER
.. <svg>
..   <path stroke="red" d="M 10 10 V 50 Z"/>
.. </svg>
.. END
