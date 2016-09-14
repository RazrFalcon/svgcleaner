Simplify transform matrices
---------------------------

Simplify transform matrices into short equivalent when possible.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <circle fill="green" cx="50"
..           cy="50" r="45"
..           transform="matrix(1 0 0 1 25 0)"/>
.. </svg>
.. AFTER
.. <svg>
..   <circle fill="green" cx="50"
..           cy="50" r="45"
..           transform="translate(25)"/>
.. </svg>
.. END
