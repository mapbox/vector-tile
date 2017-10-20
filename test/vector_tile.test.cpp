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
    REQUIRE(fm.size() == 1);
    REQUIRE(fm.end() != fm.find("layer_name"));
    auto fc = fm["layer_name"];
    REQUIRE(fc.size() == 1);
    auto f = fc[0];
    REQUIRE(f.id.is<std::uint64_t>());
    CHECK(f.id.get<std::uint64_t>() == 123);
    REQUIRE(f.properties.size() == 1);
    REQUIRE(f.properties.end() != f.properties.find("hello"));
    auto val = f.properties["hello"];
    REQUIRE(val.is<std::string>());
    CHECK(val.get<std::string>() == "world");
    REQUIRE(f.geometry.is<mapbox::geometry::point<std::int64_t>>());
    auto pt = f.geometry.get<mapbox::geometry::point<std::int64_t>>();
    CHECK(pt.x == 25);
    CHECK(pt.y == 17);
}

TEST_CASE( "Read Feature-single-multipoint.mvt" ) {
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/valid/Feature-single-multipoint.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    REQUIRE(fm.size() == 1);
    REQUIRE(fm.end() != fm.find("layer_name"));
    auto fc = fm["layer_name"];
    REQUIRE(fc.size() == 1);
    auto f = fc[0];
    REQUIRE(f.id.is<std::uint64_t>());
    CHECK(f.id.get<std::uint64_t>() == 123);
    REQUIRE(f.properties.size() == 1);
    REQUIRE(f.properties.end() != f.properties.find("hello"));
    auto val = f.properties["hello"];
    REQUIRE(val.is<std::string>());
    CHECK(val.get<std::string>() == "world");
    REQUIRE(f.geometry.is<mapbox::geometry::multi_point<std::int64_t>>());
    auto mp = f.geometry.get<mapbox::geometry::multi_point<std::int64_t>>();
    CHECK(mp[0].x == 5);
    CHECK(mp[0].y == 7);
    CHECK(mp[1].x == 3);
    CHECK(mp[1].y == 2);
}

/*
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


