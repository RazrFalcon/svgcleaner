Remove inheritable gradient attributes
--------------------------------------

Gradients can inherit attributes via ``xlink:href`` attribute, so we can
remove attributes that already defined in the parent gradient.

Currently, only an ``gradientUnits`` attribute is processed.

**Unsupported by:** QtSvg <= 5.7, Inkscape <= 0.91 r13725

.. GEN_TABLE
.. BEFORE
.. <svg>
..     <linearGradient id="lg1"
..       gradientUnits='userSpaceOnUse'>
..       <stop offset="0"
..         stop-color="yellow"/>
..       <stop offset="1"
..         stop-color="green"/>
..     </linearGradient>
..     <linearGradient id="lg2"
..       gradientUnits='userSpaceOnUse'
..       xlink:href="#lg1"/>
..     <linearGradient id="lg3"
..       gradientUnits='userSpaceOnUse'
..       xlink:href="#lg2"/>
..     <radialGradient id="rg1"
..       gradientUnits='userSpaceOnUse'
..       xlink:href="#lg3"/>
..   <circle fill="url(#rg1)"
..     cx="50" cy="50" r="45"/>
.. </svg>
.. AFTER
.. <svg>
..     <linearGradient id="lg1"
..       gradientUnits='userSpaceOnUse'>
..       <stop offset="0"
..         stop-color="yellow"/>
..       <stop offset="1"
..         stop-color="green"/>
..     </linearGradient>
..     <linearGradient id="lg2"
..       xlink:href="#lg1"/>
..     <linearGradient id="lg3"
..       xlink:href="#lg2"/>
..     <radialGradient id="rg1"
..       xlink:href="#lg3"/>
..   <circle fill="url(#rg1)"
..     cx="50" cy="50" r="45"/>
.. </svg>
.. END
