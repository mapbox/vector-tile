#include <vector_tile.hpp>
#include <iostream>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE( "Protobuf Tag Constants" ) {
    CHECK(mapbox::vector_tile::Tile::LAYERS == 3);

    CHECK(mapbox::vector_tile::Layer::VERSION == 15);
    CHECK(mapbox::vector_tile::Layer::NAME == 1);
    CHECK(mapbox::vector_tile::Layer::FEATURES == 2);
    CHECK(mapbox::vector_tile::Layer::KEYS == 3);
    CHECK(mapbox::vector_tile::Layer::VALUES == 4);
    CHECK(mapbox::vector_tile::Layer::EXTENT == 5);

    CHECK(mapbox::vector_tile::Feature::ID == 1);
    CHECK(mapbox::vector_tile::Feature::TAGS == 2);
    CHECK(mapbox::vector_tile::Feature::TYPE == 3);
    CHECK(mapbox::vector_tile::Feature::GEOMETRY == 4);
    CHECK(mapbox::vector_tile::Feature::RASTER == 5);

    CHECK(mapbox::vector_tile::Value::STRING == 1);
    CHECK(mapbox::vector_tile::Value::FLOAT == 2);
    CHECK(mapbox::vector_tile::Value::DOUBLE == 3);
    CHECK(mapbox::vector_tile::Value::INT == 4);
    CHECK(mapbox::vector_tile::Value::UINT == 5);
    CHECK(mapbox::vector_tile::Value::SINT == 6);
    CHECK(mapbox::vector_tile::Value::BOOL == 7);

    CHECK(mapbox::vector_tile::GeomType::UNKNOWN == 0);
    CHECK(mapbox::vector_tile::GeomType::POINT == 1);
    CHECK(mapbox::vector_tile::GeomType::LINESTRING == 2);
    CHECK(mapbox::vector_tile::GeomType::POLYGON == 3);
}