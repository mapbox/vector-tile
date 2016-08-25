## Vector Tile Library Demonstration

This is a command line program that shows how to decode a Mapbox Vector Tile in C++.

It uses the Mapbox C++ Vector Tile library inside the `include/` directory.

## Depends

 - C++14 compiler (>= clang++ 3.6, g++ 5)
 - make (https://www.gnu.org/software/make)

## Building

To build the demo program, called `decode`:

1) Open a terminal and navigate into the directory containing this README.md

2) Then type:

```bash
make
```

3) Then run the decode program against the same data:

```bash
./decode data/Feature-single-point.mvt
```

## Next steps

 - Look inside `decode.cpp` for examples of how to decode tiles.
 - For more usage details see the tests at https://github.com/mapbox/vector-tile/blob/master/test/unit/vector_tile.test.cpp
 - For more details about available Mapbox Vector tiles see: https://www.mapbox.com/vector-tiles/