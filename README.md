## Vector Tile Library

C++14 library for encoding & decoding [Mapbox Vector Tiles](https://www.mapbox.com/vector-tiles/).

[![Build Status](https://travis-ci.org/mapbox/vector-tile.svg?branch=master)](https://travis-ci.org/mapbox/vector-tile)

## Depends

 - C++14 compiler
 - [protozero](https://github.com/mapbox/protozero)
 - [vtzero](https://github.com/mapbox/vtzero)
 - [variant](https://github.com/mapbox/variant)
 - [geometry](https://github.com/mapbox/geometry.hpp)


## Building

Install test fixtures from an external git repository
```sh
git submodule init
git submodule update
```

Build the library
```sh
# release mode
make

# debug mode
make debug
```

Run tests
```sh
make test
```

## Demo

The demo decoder is a mini CLI for decoding Mapbox Vector Tiles and printing to stdout. The demo is built when you run `make` and can be used like:
```sh
./build/demo-decode <path_to_mvt>
```

# Who is using vector-tile?

These are the applications targeted to upgrade to this library:

* [Mapbox GL Native](https://github.com/mapbox/mapbox-gl-native)
* [Mapnik Vector Tile](https://github.com/mapbox/mapnik-vector-tile)
* [Tippecanoe](https://github.com/mapbox/tippecanoe)
