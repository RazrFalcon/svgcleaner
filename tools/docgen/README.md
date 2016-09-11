# docgen

Generate documentation for `svgcleaner`.

## Usage

Generate `svgcleaner.rst` file:
```
docgen --docdir=/svgcleaner/docs/
```

Generate documentation for svgcleaner-gui:
```
docgen --docdir=/svgcleaner/docs/ --for-gui --outdir=/tmp
```

Generated data will be then used by `svgcleaner-gui`'s `docgen`.
