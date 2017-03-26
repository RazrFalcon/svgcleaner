# How to Contribute

## New ideas

New ideas about how to make SVG files smaller are always welcome.
No need to implement them, though. Just create an issue on GitHub that explains your idea(s).

## Broken image

If you found that *svgcleaner* broke your image, firstly open them (original and cleaned) in
Google Chrome, since it has very good support of the SVG spec, unlike many other applications.

- If images still render differently - then they are probably broken.
Create an issue on GitHub with your image attached to it.
- If images are rendered the same - then there are some problems with your rendering
application/editor. Such cases are not treated as errors, but you still can open an issue.

## Performance

Note that actual cleaning takes less than 30% of the execution time.
The other 70% is spent on parsing and saving. So, if you're investigating performance issues,
you should start there.

## Other

- English grammar fixes are extremely welcome.

If you don't have a GitHub account you can send an email to: svgcleanerteam@gmail.com
