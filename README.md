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

## Demo decoder

The demo decoder is a mini CLI for decoding Mapbox Vector Tiles and printing to stdout. The demo is built when you run `make`.

Usage:

```sh
./build/demo-decode <path_to_mvt>

# example
./build/demo-decode ~/data/cool.mvt
```

The output will look like this:

```
Layer "countries" (84 features)
  Feature (id: 652891810) (type: Polygon)
    name: Ghana
    iso_2: gh
    iso_3: gha
  Feature (id: 652891810) (type: Polygon)
    name: Canada
    iso_2: ca
    iso_3: can
  ...
```

# Who is using vector-tile?

These are the applications targeted to upgrade to this library:

* [Mapbox GL Native](https://github.com/mapbox/mapbox-gl-native)
* [Mapnik Vector Tile](https://github.com/mapbox/mapnik-vector-tile)
* [Tippecanoe](https://github.com/mapbox/tippecanoe)
