Remove unused text related attributes
-------------------------------------

We can remove a text related attributes, when there are no text.

But since attributes like a `font` can impact a `length` values with an `em`/`ex` units
- it's a bit more complicated. Also the text itself can be defined in many different ways.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <circle fill="green" font="Verdana"
..           cx="50" cy="50" r="45"/>
..   <text y="30" x="30" font-size="14pt">
..     Text
..   </text>
.. </svg>
.. AFTER
.. <svg>
..   <circle fill="green"
..           cx="50" cy="50" r="45"/>
..   <text y="30" x="30" font-size="14pt">
..     Text
..   </text>
.. </svg>
.. END
