# Changelog

## 1.0.2

- Add support for QNX 7 compiler.

## 1.0.1

- Limit possibility rare situation where it might be possible to have an overflow or underflow of integers while decoding a vector tile's geometry

## 1.0.0

- Allow nulls from property parsing
- Several bug fixes around geometry parsing to prevent over allocation of memory.

## 1.0.0-alpha.1

- Added demo application showing usage of decoder

## 1.0.0-rc4

- Add decoder implementation

## 1.0.0-rc3

- expose a node.js-style `require()` so that other projects can dynamically determine the path to the headers, exactly where npm has decided to install them (sensitive to deduping).

## 1.0.0-rc2

- `#include` paths are now "quoted"
- `package.json` for publishing `.hpp` headers to npm

## 1.0.0-rc1

- `vector-tile` initial release, which includes enum classes referencing the Mapbox Vector Tile Specification protobuf tags. [#2](https://github.com/mapbox/vector-tile/issues/2).
