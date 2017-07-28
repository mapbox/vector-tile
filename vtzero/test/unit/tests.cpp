
#include <fstream>
#include <stdexcept>
#include <string>

#include <vtzero/geometry.hpp>
#include <vtzero/reader.hpp>

#include <catch.hpp>

static std::string open_tile(const std::string& path) {
    std::ifstream stream{std::string{"../../test/mvt-fixtures/fixtures/"} + path.c_str(), std::ios_base::in|std::ios_base::binary};
    if (!stream.is_open()) {
        throw std::runtime_error{"could not open: '" + path + "'"};
    }
    std::string message{std::istreambuf_iterator<char>(stream.rdbuf()),
                        std::istreambuf_iterator<char>()};
    stream.close();
    return message;
}

template <int Dimensions>
struct point_handler {

    std::vector<vtzero::point<Dimensions>> data;

    void points_begin(uint32_t count) {
        data.reserve(count);
    }

    void points_point(const vtzero::point<Dimensions> point) {
        data.push_back(point);
    }

    void points_end() const noexcept {
    }

};

template <int Dimensions>
struct linestring_handler {

    std::vector<std::vector<vtzero::point<Dimensions>>> data;

    void linestring_begin(uint32_t count) {
        data.emplace_back();
        data.back().reserve(count);
    }

    void linestring_point(const vtzero::point<Dimensions> point) {
        data.back().push_back(point);
    }

    void linestring_end() const noexcept {
    }

};

template <int Dimensions>
struct polygon_handler {

    std::vector<std::vector<vtzero::point<Dimensions>>> data;

    void ring_begin(uint32_t count) {
        data.emplace_back();
        data.back().reserve(count);
    }

    void ring_point(const vtzero::point<Dimensions> point) {
        data.back().push_back(point);
    }

    void ring_end(bool) const noexcept {
    }

};

vtzero::feature check_layer(vtzero::vector_tile& tile) {
    REQUIRE(std::distance(tile.begin(), tile.end()) == 1);

    const auto layer = *tile.begin();
    REQUIRE(layer.name() == "layer_name");
    REQUIRE(layer.version() == 2);
    REQUIRE(layer.extent() == 4096);
    REQUIRE(layer.get_feature_count() == 1);

    const auto feature = *layer.begin();
    REQUIRE(feature.id() == 123);

    return feature;
}

TEST_CASE("Read Feature-single-point.mvt") {
    std::string buffer = open_tile("valid/Feature-single-point.mvt");
    vtzero::vector_tile tile{buffer};

    const auto feature = check_layer(tile);

    REQUIRE(feature.type() == vtzero::GeomType::POINT);

    point_handler<2> handler;
    vtzero::decode_point_geometry(feature.geometry(), true, handler);

    std::vector<vtzero::point<2>> result = {{25, 17}};
    REQUIRE(handler.data == result);
}

TEST_CASE("Read Feature-single-multipoint.mvt") {
    std::string buffer = open_tile("valid/Feature-single-multipoint.mvt");
    vtzero::vector_tile tile{buffer};

    const auto feature = check_layer(tile);

    REQUIRE(feature.type() == vtzero::GeomType::POINT);

    point_handler<2> handler;
    vtzero::decode_point_geometry(feature.geometry(), true, handler);

    std::vector<vtzero::point<2>> result = {{5, 7}, {3,2}};
    REQUIRE(handler.data == result);
}

TEST_CASE("Read Feature-single-linestring.mvt") {
    std::string buffer = open_tile("valid/Feature-single-linestring.mvt");
    vtzero::vector_tile tile{buffer};

    const auto feature = check_layer(tile);

    REQUIRE(feature.type() == vtzero::GeomType::LINESTRING);

    linestring_handler<2> handler;
    vtzero::decode_linestring_geometry(feature.geometry(), true, handler);

    std::vector<std::vector<vtzero::point<2>>> result = {{{2, 2}, {2,10}, {10, 10}}};
    REQUIRE(handler.data == result);
}

TEST_CASE("Read Feature-single-multilinestring.mvt") {
    std::string buffer = open_tile("valid/Feature-single-multilinestring.mvt");
    vtzero::vector_tile tile{buffer};

    const auto feature = check_layer(tile);

    REQUIRE(feature.type() == vtzero::GeomType::LINESTRING);

    linestring_handler<2> handler;
    vtzero::decode_linestring_geometry(feature.geometry(), true, handler);

    std::vector<std::vector<vtzero::point<2>>> result = {{{2, 2}, {2,10}, {10, 10}}, {{1,1}, {3, 5}}};
    REQUIRE(handler.data == result);
}

TEST_CASE("Read Feature-single-polygon.mvt") {
    std::string buffer = open_tile("valid/Feature-single-polygon.mvt");
    vtzero::vector_tile tile{buffer};

    const auto feature = check_layer(tile);

    REQUIRE(feature.type() == vtzero::GeomType::POLYGON);

    polygon_handler<2> handler;
    vtzero::decode_polygon_geometry(feature.geometry(), true, handler);

    std::vector<std::vector<vtzero::point<2>>> result = {{{3, 6}, {8,12}, {20, 34}, {3, 6}}};
    REQUIRE(handler.data == result);
}

TEST_CASE("Read Feature-unknown-GeomType.mvt") {
    std::string buffer = open_tile("valid/Feature-unknown-GeomType.mvt");
    vtzero::vector_tile tile{buffer};

    const auto feature = check_layer(tile);

    REQUIRE(feature.type() == vtzero::GeomType::UNKNOWN);
}

