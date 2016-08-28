Remove an unused ``xmlns:xlink`` attribute
------------------------------------------

We can remove a ``xmlns:xlink`` attribute if document doesn't use an element
referencing via the ``xlink:href``.

.. GEN_TABLE
.. NO_XMLNS_XLINK
.. BEFORE
.. <svg xmlns:xlink="http://www.w3.org/1999/xlink">
..   <circle fill="green" cx="50" cy="50" r="45"/>
.. </svg>
.. AFTER
.. <svg>
..   <circle fill="green" cx="50" cy="50" r="45"/>
.. </svg>
.. END
