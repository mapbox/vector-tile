#include <mapbox/vector_tile.hpp>
#include <iostream>
#include <fstream>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

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

std::string open_tile(std::string const& path) {
    std::ifstream stream(path.c_str(),std::ios_base::in|std::ios_base::binary);
    if (!stream.is_open())
    {
        throw std::runtime_error("could not open: '" + path + "'");
    }
    std::string message(std::istreambuf_iterator<char>(stream.rdbuf()),(std::istreambuf_iterator<char>()));
    stream.close();
    return message;
}

TEST_CASE( "Read tile" ) {
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/valid/Feature-single-point.mvt");
    mapbox::vector_tile::buffer tile(buffer);
    const auto layer_names = tile.layerNames();
    REQUIRE(layer_names.size() == 1);
    REQUIRE(layer_names[0] == "layer_name");
    auto layer_ptr = tile.getLayer("layer_name");
    REQUIRE(layer_ptr != nullptr);
    const mapbox::vector_tile::layer & layer = *layer_ptr;
    REQUIRE(layer.featureCount() == 1);
    REQUIRE(layer.getName() == "layer_name");
    auto feature_ptr = layer.getFeature(0);
    const mapbox::vector_tile::feature & feature = * feature_ptr;
    auto feature_id = feature.getID();
    REQUIRE(feature_id);
    REQUIRE(feature_id->is<uint64_t>());
    REQUIRE(feature_id->get<uint64_t>() == 123ull);
    REQUIRE(feature.getType() == mapbox::vector_tile::GeomType::POINT);
    auto props = feature.getProperties();
    auto val = props["hello"];
    REQUIRE(val.is<std::string>());
    REQUIRE(val.get<std::string>() == "world");
    auto opt_val = feature.getValue("hello");
    REQUIRE(opt_val);
    REQUIRE(opt_val->is<std::string>());
    REQUIRE(opt_val->get<std::string>() == "world");
}


