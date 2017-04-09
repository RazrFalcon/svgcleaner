Remove ``version`` and ``baseProfile`` attributes
-------------------------------------------------

Remove ``version`` and ``baseProfile`` attributes from the ``svg`` element.

Some applications can rely on them, so someone may want to keep them.
Even throw they usually useless.

.. GEN_TABLE
.. BEFORE
.. <svg version="1.1" baseProfile="tiny">
..   <circle fill="green"
..           cx="50" cy="50" r="45"/>
.. </svg>
.. AFTER
.. <svg>
..   <circle fill="green"
..           cx="50" cy="50" r="45"/>
.. </svg>
.. END
