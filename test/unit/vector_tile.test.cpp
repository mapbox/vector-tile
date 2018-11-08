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

#define ASSERT_KNOWN_FEATURE() \
    auto const layer_names = tile.layerNames(); \
    REQUIRE(layer_names.size() == 1); \
    REQUIRE(layer_names[0] == "layer_name"); \
    auto const layer = tile.getLayer("layer_name"); \
    REQUIRE(layer.featureCount() == 1); \
    REQUIRE(layer.getName() == "layer_name"); \
    auto const feature = mapbox::vector_tile::feature(layer.getFeature(0),layer); \
    auto const& feature_id = feature.getID(); \
    REQUIRE(feature_id.is<uint64_t>()); \
    REQUIRE(feature_id.get<uint64_t>() == 123ull); \
    auto props = feature.getProperties(); \
    auto itr = props.find("hello"); \
    REQUIRE(itr != props.end()); \
    auto const& val = itr->second; \
    REQUIRE(val.is<std::string>()); \
    REQUIRE(val.get<std::string>() == "world"); \
    auto opt_val = feature.getValue("hello"); \
    REQUIRE(opt_val.is<std::string>()); \
    REQUIRE(opt_val.get<std::string>() == "world"); \
    mapbox::vector_tile::points_arrays_type geom = feature.getGeometries<mapbox::vector_tile::points_arrays_type>(1.0);

std::string stringify_geom(mapbox::vector_tile::points_arrays_type const& geom) {
    std::stringstream s;
    for (auto const& point_array : geom) {
        s << "[";
        for (auto const& point : point_array) {
            s << point.x << ", " << point.y;
        }
        s << "]";
    }
    return s.str();
}

TEST_CASE( "Read Feature-single-point.mvt" ) {
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/valid/Feature-single-point.mvt");
    mapbox::vector_tile::buffer tile(buffer);
    ASSERT_KNOWN_FEATURE()
    REQUIRE(feature.getType() == mapbox::vector_tile::GeomType::POINT); \
    REQUIRE(stringify_geom(geom) == "[25, 17]");
}

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

/*TEST_CASE( "Read Feature-single-multipolygon.mvt" ) {
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/valid/Feature-single-multipolygon.mvt");
    mapbox::vector_tile::buffer tile(buffer);
    ASSERT_KNOWN_FEATURE()
    REQUIRE(feature.getType() == mapbox::vector_tile::GeomType::POINT); \
    REQUIRE(stringify_geom(geom) == "25, 17");
}*/

TEST_CASE( "Prevent massive over allocation" ) {
    std::string buffer = open_tile("test/test046.mvt");
    mapbox::vector_tile::buffer tile(buffer);
    auto const layer_names = tile.layerNames();
    REQUIRE(layer_names.size() == 1);
    REQUIRE(layer_names[0] == "0000000000");
    auto const layer = tile.getLayer("0000000000");
    REQUIRE(layer.featureCount() == 1);
    REQUIRE(layer.getName() == "0000000000");
    auto const feature = mapbox::vector_tile::feature(layer.getFeature(0),layer);
    mapbox::vector_tile::points_arrays_type geom = feature.getGeometries<mapbox::vector_tile::points_arrays_type>(1.0);
    REQUIRE(geom.capacity() <= 655360);
}
