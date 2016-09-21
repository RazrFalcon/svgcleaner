Remove duplicated ``feGaussianBlur`` elements
---------------------------------------------

An SVG can contain a lot of ``feGaussianBlur`` elements, which may render exactly the same.
So we can remove duplicates and update links in elements, that uses them.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <defs>
..     <filter id='f1'>
..       <feGaussianBlur stdDeviation='2'/>
..     </filter>
..     <filter id='f2'>
..       <feGaussianBlur stdDeviation='2'/>
..     </filter>
..   </defs>
..   <circle filter="url(#f1)" fill="green"
..           cx="50" cy="50" r="45"/>
..   <circle filter="url(#f2)" fill="green"
..           cx="100" cy="50" r="45"/>
.. </svg>
.. AFTER
.. <svg>
..   <defs>
..     <filter id='f1'>
..       <feGaussianBlur stdDeviation='2'/>
..     </filter>
..   </defs>
..   <circle filter="url(#f1)" fill="green"
..           cx="50" cy="50" r="45"/>
..   <circle filter="url(#f1)" fill="green"
..           cx="100" cy="50" r="45"/>
.. </svg>
.. END
