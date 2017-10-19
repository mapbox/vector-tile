#include <mapbox/vector_tile.hpp>
#include <mapbox/vector_tile/version.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#include <catch.hpp>

static std::string open_tile(std::string const& path) {
    std::ifstream stream(path.c_str(),std::ios_base::in|std::ios_base::binary);
    if (!stream.is_open())
    {
        throw std::runtime_error("could not open: '" + path + "'");
    }
    std::string message(std::istreambuf_iterator<char>(stream.rdbuf()),(std::istreambuf_iterator<char>()));
    stream.close();
    return message;
}

TEST_CASE( "Read Feature-single-point.mvt" ) {
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/valid/Feature-single-point.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
}

/*
TEST_CASE( "Read Feature-single-multipoint.mvt" ) {
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/valid/Feature-single-multipoint.mvt");
    mapbox::vector_tile::buffer tile(buffer);
    ASSERT_KNOWN_FEATURE()
    REQUIRE(feature.getType() == mapbox::vector_tile::GeomType::POINT); \
    REQUIRE(stringify_geom(geom) == "[5, 7][3, 2]");
}

TEST_CASE( "Read Feature-single-linestring.mvt" ) {
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/valid/Feature-single-linestring.mvt");
    mapbox::vector_tile::buffer tile(buffer);
    ASSERT_KNOWN_FEATURE()
    REQUIRE(feature.getType() == mapbox::vector_tile::GeomType::LINESTRING); \
    REQUIRE(stringify_geom(geom) == "[2, 22, 1010, 10]");
}

TEST_CASE( "Read Feature-single-multilinestring.mvt" ) {
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/valid/Feature-single-multilinestring.mvt");
    mapbox::vector_tile::buffer tile(buffer);
    ASSERT_KNOWN_FEATURE()
    REQUIRE(feature.getType() == mapbox::vector_tile::GeomType::LINESTRING); \
    REQUIRE(stringify_geom(geom) == "[2, 22, 1010, 10][1, 13, 5]");
}

TEST_CASE( "Read Feature-single-polygon.mvt" ) {
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/valid/Feature-single-polygon.mvt");
    mapbox::vector_tile::buffer tile(buffer);
    ASSERT_KNOWN_FEATURE()
    REQUIRE(feature.getType() == mapbox::vector_tile::GeomType::POLYGON); \
    REQUIRE(stringify_geom(geom) == "[3, 68, 1220, 343, 6]");
}

TEST_CASE( "Read Feature-single-multipolygon.mvt" ) {
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/valid/Feature-single-multipolygon.mvt");
    mapbox::vector_tile::buffer tile(buffer);
    ASSERT_KNOWN_FEATURE()
    REQUIRE(feature.getType() == mapbox::vector_tile::GeomType::POINT); \
    REQUIRE(stringify_geom(geom) == "25, 17");
}*/


