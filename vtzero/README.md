
# vtzero - C++ Vector Tile Library

Minimal overhead C++11 library for reading and writing Mapbox vector tiles.

This is experimental code. Do not use this. 

## Dependencies

* [Protozero](https://github.com/mapbox/protozero)

## Build and run tests

To build test programs:

    cd src
    make

You might have to set `PROTOZERO_INCLUDE` in the Makefile.

Call

    ./vtzero-create

to create test tile named `test.mvt`.

Call

    ./vtzero-show TILE-FILE

to show contents of `TILE-FILE`.

## Goals

The goals of this library are:

* Minimal overhead (compared to direct use of Protozero) reading and writing of
  vector tiles.
* Concerned with the low-level vector tile encoding, not issues like deciding
  what features should end up in what form in the vector tiles. Specifically
  splitting features or generalization of data is not in scope.
* Widely usable, uses only C++11, header only, works on any OS.
* No dependeny on anything other than C++ standard lib and Protozero for the
  core of the library. Some parts may have other, optional dependencies.
* Lazy decoding of vector tiles where possible, don't do work to decode the
  tiles we don't need. Make undecoded parts available so they can be copied
  whole into other vector tiles where useful and possible.
* Avoid memory allocation and copying of data.
* Vector tiles can come from any source. Must be robust, read any valid or
  invalid vector tile without crashing.

