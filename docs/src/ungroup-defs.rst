Ungroup ``defs`` element
------------------------

If the ``defs`` element contains only `referenced <https://www.w3.org/TR/SVG/struct.html#Head>`_
elements - it can be ungrouped.

**Unsupported by:** QtSvg <= 5.7 (``pattern`` with ``image`` child renders incorrectly)

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <defs>
..     <radialGradient id="rg1">
..       <stop offset="0"
..             stop-color="yellow"/>
..       <stop offset="1"
..             stop-color="green"/>
..     </radialGradient>
..   </defs>
..   <circle fill="url(#rg1)" r="45"
..           cx="50" cy="50"/>
.. </svg>
.. AFTER
.. <svg>
..   <radialGradient id="rg1">
..     <stop offset="0"
..           stop-color="yellow"/>
..     <stop offset="1"
..           stop-color="green"/>
..   </radialGradient>
..   <circle fill="url(#rg1)" r="45"
..           cx="50" cy="50"/>
.. </svg>
.. END
