Remove unresolved classes from ``class`` attributes
---------------------------------------------------

The ``class`` attribute can contain a list of class selectors,
but not all of them may link to the style sheet defined in the file.

This option will remove such selectors.

**Note:** you can't prevent class attribute resolving anyway. This option should be used
just to keep unresolved classes in the ``class`` attribute when you define them elsewhere.
So you should disable it to get such behavior.

.. GEN_TABLE
.. BEFORE
.. <svg id="svg1">
..   <style>
..     .fill1 {fill:green}
..   </style>
..   <circle class=".fill1 .stroke1 .other"
..           cx="50" cy="50" r="50"/>
.. </svg>
.. AFTER
.. <svg>
..   <circle fill="green"
..           cx="50" cy="50" r="50"/>
.. </svg>
.. END
