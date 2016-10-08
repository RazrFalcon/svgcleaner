Remove text-related attributes if there is no text
--------------------------------------------------

We can remove text-related attributes, when there is no text.

But since attributes like a `font` can impact a `length` values with a `em`/`ex` units
- it's a bit more complicated. Also, the text itself can be defined in many different ways.

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
