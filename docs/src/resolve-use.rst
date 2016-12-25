Resolve ``use`` elements
------------------------

We can replace ``use`` element with linked element if it used only by this ``use``.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <defs>
..     <circle id='circle1'
..             fill="green" cx="50"
..             cy="50" r="45"/>
..   </defs>
..   <use xlink:href='#circle1'/>
.. </svg>
.. AFTER
.. <svg>
..   <circle id='circle1'
..           fill="green" cx="50"
..           cy="50" r="45"/>
.. </svg>
.. END
