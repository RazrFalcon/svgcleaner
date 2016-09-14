Use #RGB notation
-----------------

Use #RGB notation instead of #RRGGBB when possible.

**NOTE:** by default all color stored as #RRGGBB, since ``libsvgdom`` doesn't stores
colors original text representation.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <circle fill="#00ff00" cx="50" cy="50" r="45"/>
.. </svg>
.. AFTER
.. <svg>
..   <circle fill="#0f0" cx="50" cy="50" r="45"/>
.. </svg>
.. END
