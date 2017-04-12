Join presentational attributes
------------------------------

SVG presentational attributes can be set via separated attributes and via ``style`` attribute.
If we have less than 5 presentational attributes - it's better to store them separately.
Otherwise ``style`` is shorter.

Possible values:

- no - do not join presentational attributes
- some - join presentational attributes when there are 6 or more of them
- all - join all presentational attributes. May produce a bigger file but can be
  used as a workaround of some viewers bugs.

Default: some

.. There is no example, because a style with 5 attributes will be a huge, nonbreakable line,
.. which will break the layout.
