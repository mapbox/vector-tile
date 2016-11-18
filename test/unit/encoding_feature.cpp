#include "catch.hpp"

#include <mapbox/geometry.hpp>
#include <mapbox/vector_tile/encode_feature.hpp>

// protozero
#include <protozero/pbf_reader.hpp>

//
// Unit tests for encoding of features
//

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
inline void test_property_encoding(protozero::pbf_reader & l,
                                   mapbox::geometry::property_map & properties) {
    REQUIRE(l.next());
    REQUIRE(l.tag() == mapbox::vector_tile::layer_message::KEYS);
    auto itr = properties.find(l.get_string());
    REQUIRE(itr != properties.end());
    REQUIRE(l.next());
    REQUIRE(l.tag() == mapbox::vector_tile::layer_message::VALUES);
    auto v = l.get_message();
    REQUIRE(v.next());
    switch (v.tag()) {
        case mapbox::vector_tile::value_message::STRING:
            REQUIRE(itr->second.is<std::string>());
            REQUIRE(v.get_string() == itr->second.get<std::string>());
            break;
        case mapbox::vector_tile::value_message::FLOAT:
            REQUIRE(itr->second.is<double>());
            REQUIRE(static_cast<double>(v.get_float()) == itr->second.get<double>());
            break;
        case mapbox::vector_tile::value_message::DOUBLE:
            REQUIRE(itr->second.is<double>());
            REQUIRE(v.get_double() == itr->second.get<double>());
            break;
        case mapbox::vector_tile::value_message::INT:
            REQUIRE(itr->second.is<std::int64_t>());
            REQUIRE(v.get_int64() == itr->second.get<std::int64_t>());
            break;
        case mapbox::vector_tile::value_message::UINT:
            REQUIRE(itr->second.is<std::uint64_t>());
            REQUIRE(v.get_uint64() == itr->second.get<std::uint64_t>());
            break;
        case mapbox::vector_tile::value_message::SINT:
            REQUIRE(false); // we do not encode things as sint currently
            break;
        case mapbox::vector_tile::value_message::BOOL:
            REQUIRE(itr->second.is<bool>());
            REQUIRE(v.get_bool() == itr->second.get<bool>());
            break;
        default:
            REQUIRE(false);
            break;
    }
    properties.erase(itr);
}
#pragma clang diagnostic pop

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
    test_property_encoding(l, properties);
    test_property_encoding(l, properties);
    test_property_encoding(l, properties);
    test_property_encoding(l, properties);
    test_property_encoding(l, properties);
    REQUIRE(l.next());
    REQUIRE(l.tag() == mapbox::vector_tile::layer_message::FEATURES);
    auto f = l.get_message();
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::feature_message::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::geom_type::POINT);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::feature_message::GEOMETRY);
    auto geometry_iter = f.get_packed_uint32();
    auto itr = geometry_iter.begin();
    const auto end_itr = geometry_iter.end();
    
    // MoveTo, ParameterInteger, ParameterInteger
    // Therefore 1 commands + 2 parameters = 3
    REQUIRE(std::distance(itr, end_itr) == 3);
    // MoveTo(10,10)
    CHECK(*itr++ == ((1 << 3) | 1u)); // 9
    CHECK(*itr++ == 20);
    CHECK(*itr++ == 20);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::feature_message::ID);
    CHECK(f.get_uint64() == 100);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::feature_message::TAGS);
    auto tags_iter = f.get_packed_uint32();
    REQUIRE(std::distance(tags_iter.begin(), tags_iter.end()) == 10);
    // Not checking that all the tags are correct -- but might be in slightly
    // different order based on hashing so not specifying in test.
    REQUIRE_FALSE(f.next());
    REQUIRE_FALSE(l.next());
}
