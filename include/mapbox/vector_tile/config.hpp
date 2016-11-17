#pragma once

#include <cstdint>

namespace mapbox { namespace vector_tile {


enum tile_message : std::uint8_t
{
    LAYERS = 3
};

enum layer_message : std::uint8_t
{
    VERSION = 15,
    NAME = 1,
    FEATURES = 2,
    KEYS = 3,
    VALUES = 4,
    EXTENT = 5
};

enum feature_message : std::uint8_t
{
    ID = 1,
    TAGS = 2,
    TYPE = 3,
    GEOMETRY = 4,
    RASTER = 5
};

enum value_message : std::uint32_t
{
    STRING = 1,
    FLOAT = 2,
    DOUBLE = 3,
    INT = 4,
    UINT = 5,
    SINT = 6,
    BOOL = 7
};

enum geom_type : std::uint8_t
{
    UNKNOWN = 0,
    POINT = 1,
    LINESTRING = 2,
    POLYGON = 3
};

enum command_type : std::uint8_t
{
    END = 0,
    MOVE_TO = 1,
    LINE_TO = 2,
    CLOSE = 7
};


} }
