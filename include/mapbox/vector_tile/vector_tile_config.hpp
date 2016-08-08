#include <iostream>

namespace vector_tile
{

namespace Encoding
{

/*
writer.add_uint32(vector_tile::Encoding::Tile::VERSION, 2);
*/
enum Tile : std::uint32_t
{
    LAYERS = 3
};

enum Layer : std::uint32_t
{
    VERSION = 15,
    NAME = 1,
    FEATURES = 2,
    KEYS = 3,
    VALUES = 4,
    EXTENT = 5
};

enum Feature : std::uint32_t
{
    ID = 1,
    TAGS = 2,
    TYPE = 3,
    GEOMETRY = 4,
    RASTER = 5
};

enum Value : std::uint32_t
{
    STRING = 1,
    FLOAT = 2,
    DOUBLE = 3,
    INT = 4,
    UINT = 5,
    SINT = 6,
    BOOL = 7
};

enum GeomType : std::uint8_t
{
    UNKNOWN = 0,
    POINT = 1,
    LINESTRING = 2,
    POLYGON = 3
};

} // end ns Encoding

} // end ns vector_tile