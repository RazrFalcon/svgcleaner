Set path's coordinates numeric precision
----------------------------------------

We can reduce the numeric precision of path's coordinates without breaking it.

Range: 1..12, where
 - 8..12 is basically lossless
 - 4..7 will give an actual impact on the file size
 - 1..3 is **very dangerous** and will probably break your file

Default: 8

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <path d="M 10.000001 10.000005
..            L 89.99999 10.11111
..            L 89.997777 90.0005
..            L 10.123456789 90 L 10 10 z"
..         fill="none" stroke="red"/>
.. </svg>
.. AFTER
.. <svg>
..   <path d="M 10 10.00001
..            L 89.99999 10.11111
..            L 89.99778 90.0005
..            L 10.12346 90 L 10 10 z"
..         fill="none" stroke="red"/>
.. </svg>
.. END
