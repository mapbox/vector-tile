# Changelog

## 1.0.0-rc3

- expose a node.js-style `require()` for usage in other projects that must avoid duplicate binaries from the same project (because npm installs every dependency without deduping).

## 1.0.0-rc2

- `#include` paths are now "quoted"
- `package.json` for publishing `.hpp` headers to npm

## 1.0.0-rc1

- `vector-tile` initial release, which includes enum classes referencing the Mapbox Vector Tile Specification protobuf tags. [#2](https://github.com/mapbox/vector-tile/issues/2).
