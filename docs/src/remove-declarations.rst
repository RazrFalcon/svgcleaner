Remove XML declarations
-----------------------

Removes XML declarations from SVG document.

``svgcleaner`` will remove all declarations, even though they are only allowed
at the start of the document.

.. GEN_TABLE
.. BEFORE
.. <?xml version="1.0" encoding="UTF-8" standalone="no"?>
.. <svg/>
.. AFTER
.. <svg/>
.. END
