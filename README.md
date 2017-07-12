## Vector Tile Library

C++14 library for decoding [Mapbox Vector Tiles](https://www.mapbox.com/vector-tiles/).

[![Build Status](https://travis-ci.org/mapbox/vector-tile.svg?branch=master)](https://travis-ci.org/mapbox/vector-tile)

## Depends

 - C++14 compiler
 - [protozero](https://github.com/mapbox/protozero)
 - [variant](https://github.com/mapbox/variant)
 - [geometry](https://github.com/mapbox/geometry.hpp)


## Building

Call
```sh
git submodule init
git submodule update
```

to install test fixtures from an external git repository.

To install all dependencies and build the tests in this repo do:

```sh
make test
```

## To bundle the `demo` program do:

```sh
make demo
```

This copies all the includes into the `demo/include` folder such that the demo can be build like:

```sh
make -C demo/
```

Or also like:

```sh
cd demo
make
```

# Who is using vector-tile?

These are the applications targeted to upgrade to this library:

* [Mapbox GL Native](https://github.com/mapbox/mapbox-gl-native)
* [Mapnik Vector Tile](https://github.com/mapbox/mapnik-vector-tile)
* [Tippecanoe](https://github.com/mapbox/tippecanoe)
