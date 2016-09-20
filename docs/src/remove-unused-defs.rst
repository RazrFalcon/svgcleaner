Remove unused referenced elements
---------------------------------

We can remove any `referenced <https://www.w3.org/TR/SVG/struct.html#Head>`_ elements from the
SVG document if no other elements are linked to them.

A link can be established via IRI or `FuncIRI <https://www.w3.org/TR/SVG/linking.html#IRIforms>`_.

Also, we can remove any unreferenced elements inside the `defs` elements,
since they are not rendered either way.

**Note:** the ``font-face`` element should be ignored, because it applies to the whole
document and not to a specific node.

.. GEN_TABLE
.. BEFORE
.. <svg>
..   <defs>
..     <g fill="red">
..       <circle id="circle1" fill="url(#rg1)"
..               cx="50" cy="50" r="50"/>
..       <circle id="circle2" fill="url(#rg2)"
..               cx="50" cy="50" r="50"/>
..     </g>
..     <radialGradient id="rg1">
..       <stop offset="0" stop-color="yellow"/>
..       <stop offset="1" stop-color="green"/>
..     </radialGradient>
..     <radialGradient id="rg2">
..       <stop offset="0" stop-color="red"/>
..       <stop offset="1" stop-color="blue"/>
..     </radialGradient>
..   </defs>
..   <use xlink:href="#circle1"/>
.. </svg>
.. AFTER
.. <svg>
..   <defs>
..     <circle id="circle1" fill="url(#rg1)"
..             cx="50" cy="50" r="50"/>
..     <radialGradient id="rg1">
..       <stop offset="0" stop-color="yellow"/>
..       <stop offset="1" stop-color="green"/>
..     </radialGradient>
..     </defs>
..   <use xlink:href="#circle1"/>
.. </svg>
.. END
