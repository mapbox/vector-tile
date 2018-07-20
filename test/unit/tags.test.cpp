#include <mapbox/vector_tile.hpp>
#include <mapbox/vector_tile/version.hpp>

#include <catch.hpp>

TEST_CASE( "Version constant" ) {
    CHECK(std::string(VECTOR_TILE_VERSION_STRING) == std::string("1.0.2"));
    CHECK(VECTOR_TILE_VERSION_CODE == 10002);
}

TEST_CASE( "Protobuf Tag Constants" ) {
    CHECK(mapbox::vector_tile::TileType::LAYERS == 3);

    CHECK(mapbox::vector_tile::LayerType::VERSION == 15);
    CHECK(mapbox::vector_tile::LayerType::NAME == 1);
    CHECK(mapbox::vector_tile::LayerType::FEATURES == 2);
    CHECK(mapbox::vector_tile::LayerType::KEYS == 3);
    CHECK(mapbox::vector_tile::LayerType::VALUES == 4);
    CHECK(mapbox::vector_tile::LayerType::EXTENT == 5);

    CHECK(mapbox::vector_tile::FeatureType::ID == 1);
    CHECK(mapbox::vector_tile::FeatureType::TAGS == 2);
    CHECK(mapbox::vector_tile::FeatureType::TYPE == 3);
    CHECK(mapbox::vector_tile::FeatureType::GEOMETRY == 4);
    CHECK(mapbox::vector_tile::FeatureType::RASTER == 5);

    CHECK(mapbox::vector_tile::ValueType::STRING == 1);
    CHECK(mapbox::vector_tile::ValueType::FLOAT == 2);
    CHECK(mapbox::vector_tile::ValueType::DOUBLE == 3);
    CHECK(mapbox::vector_tile::ValueType::INT == 4);
    CHECK(mapbox::vector_tile::ValueType::UINT == 5);
    CHECK(mapbox::vector_tile::ValueType::SINT == 6);
    CHECK(mapbox::vector_tile::ValueType::BOOL == 7);

    CHECK(mapbox::vector_tile::GeomType::UNKNOWN == 0);
    CHECK(mapbox::vector_tile::GeomType::POINT == 1);
    CHECK(mapbox::vector_tile::GeomType::LINESTRING == 2);
    CHECK(mapbox::vector_tile::GeomType::POLYGON == 3);
}
