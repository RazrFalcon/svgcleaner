Remove ``metadata`` element
---------------------------

We can remove all `metadata <https://www.w3.org/TR/SVG/metadata.html#MetadataElement>`_
elements since they are not rendered either way.

But since this element can be used by render software - this action is optional.

.. GEN_TABLE
.. BEFORE
.. <svg xmlns:dc="http://purl.org/dc/elements/1.1/"
..      xmlns:cc="http://creativecommons.org/ns#"
..      xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#">
..   <metadata id="metadata1">
..     <rdf:RDF>
..       <cc:Work rdf:about="">
..         <dc:format>image/svg+xml</dc:format>
..         <dc:type
..           rdf:resource="http://purl.org/dc/dcmitype/StillImage"/>
..         <dc:title/>
..       </cc:Work>
..     </rdf:RDF>
..   </metadata>
..   <circle fill="green" cx="50" cy="50" r="45"/>
.. </svg>
.. AFTER
.. <svg xmlns:dc="http://purl.org/dc/elements/1.1/"
..      xmlns:cc="http://creativecommons.org/ns#"
..      xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#">
..   <circle fill="green" cx="50" cy="50" r="45"/>
.. </svg>
.. END
