#include "catch.hpp"

#include <mapbox/geometry.hpp>
#include <mapbox/vector_tile/encode_feature.hpp>

// protozero
#include <protozero/pbf_reader.hpp>

//
// Unit tests for encoding of features
//
TEST_CASE("encode simple point feature") {

    mapbox::geometry::point<std::int64_t> point(10,10);
    mapbox::geometry::property_map properties;
    properties.emplace("null", mapbox::geometry::null_value_t());
    properties.emplace("bool", true);
    properties.emplace("uint", static_cast<uint64_t>(1));
    properties.emplace("int", static_cast<int64_t>(2));
    properties.emplace("double", static_cast<double>(3.14));
    properties.emplace("string", std::string("foo"));
    std::experimental::optional<mapbox::geometry::identifier> id(static_cast<int64_t>(100));
    
    mapbox::geometry::feature<std::int64_t> feature = { point, properties, id };

    mapbox::vector_tile::layer_keys_container layer_keys;
    mapbox::vector_tile::layer_values_container layer_values;
    std::string buffer;

    mapbox::vector_tile::encode_feature(buffer, layer_keys, layer_values, feature);
    
    protozero::pbf_reader l(buffer);
    REQUIRE(l.next()); 
}
