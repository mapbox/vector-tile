#include "catch.hpp"

#include <mapbox/geometry.hpp>
#include <mapbox/vector_tile/encode_feature.hpp>

// protozero
#include <protozero/pbf_reader.hpp>

#include <limits>

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

TEST_CASE("encode simple multi point feature") {

    mapbox::geometry::multi_point<std::int64_t> mp;
    mp.emplace_back(10,10);
    mp.emplace_back(20,20);
    mp.emplace_back(30,30);
    mapbox::geometry::property_map properties;
    properties.emplace("null", mapbox::geometry::null_value_t());
    properties.emplace("bool", false);
    properties.emplace("uint", std::numeric_limits<std::uint64_t>::max());
    properties.emplace("int", std::numeric_limits<std::int64_t>::max());
    properties.emplace("double", std::numeric_limits<double>::max());
    properties.emplace("string", std::string("fee"));
    std::experimental::optional<mapbox::geometry::identifier> id(static_cast<int64_t>(100));
    
    mapbox::geometry::feature<std::int64_t> feature = { mp, properties, id };

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
    // MoveTo, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger
    // Therefore 1 commands + 6 parameters = 7
    REQUIRE( std::distance(itr, end_itr) == 7);
    // MoveTo(10,10)
    CHECK(*itr++ == ((3 << 3) | 1u)); // 25
    CHECK(*itr++ == 20);
    CHECK(*itr++ == 20);
    // MoveTo(10,10)
    CHECK(*itr++ == 20);
    CHECK(*itr++ == 20);
    // MoveTo(20,20)
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

TEST_CASE("encode simple line feature") {
    mapbox::geometry::line_string<std::int64_t> line;
    line.emplace_back(10,10);
    line.emplace_back(20,20);
    line.emplace_back(30,30);
    mapbox::geometry::property_map properties;
    properties.emplace("null", mapbox::geometry::null_value_t());
    properties.emplace("bool", false);
    properties.emplace("uint", std::numeric_limits<std::uint64_t>::min());
    properties.emplace("int", std::numeric_limits<std::int64_t>::min());
    properties.emplace("double", std::numeric_limits<double>::min());
    properties.emplace("string", std::string("fee"));
    std::experimental::optional<mapbox::geometry::identifier> id(static_cast<int64_t>(100));
    
    mapbox::geometry::feature<std::int64_t> feature = { line, properties, id };

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
    REQUIRE(f.get_enum() == mapbox::vector_tile::geom_type::LINESTRING);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::feature_message::GEOMETRY);
    auto geometry_iter = f.get_packed_uint32();
    auto itr = geometry_iter.begin();
    const auto end_itr = geometry_iter.end();
    // MoveTo, ParameterInteger, ParameterInteger
    // LineTo, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger
    // Therefore 2 commands + 6 parameters = 8
    REQUIRE(std::distance(itr, end_itr) == 8);
    // MoveTo(10,10)
    CHECK(*itr++ == ((1 << 3) | 1u)); // 9
    CHECK(*itr++ == 20);
    CHECK(*itr++ == 20);
    // LineTo(20,20)
    CHECK(*itr++ == ((2 << 3) | 2u)); 
    CHECK(*itr++ == 20);
    CHECK(*itr++ == 20);
    // LineTo(30,30)
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

TEST_CASE("encode simple multi line feature") {
    mapbox::geometry::multi_line_string<std::int64_t> g;
    mapbox::geometry::line_string<std::int64_t> l1;
    l1.emplace_back(0,0);
    l1.emplace_back(1,1);
    l1.emplace_back(2,2);
    g.push_back(std::move(l1));
    mapbox::geometry::line_string<std::int64_t> l2;
    l2.emplace_back(5,5);
    l2.emplace_back(0,0);
    g.push_back(std::move(l2));
    mapbox::geometry::property_map properties;
    properties.emplace("null", mapbox::geometry::null_value_t());
    properties.emplace("bool", false);
    properties.emplace("uint", std::numeric_limits<std::uint64_t>::min());
    properties.emplace("int", std::numeric_limits<std::int64_t>::min());
    properties.emplace("double", std::numeric_limits<double>::min());
    properties.emplace("string", std::string("fee"));
    std::experimental::optional<mapbox::geometry::identifier> id(static_cast<int64_t>(100));
    
    mapbox::geometry::feature<std::int64_t> feature = { g, properties, id };

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
    REQUIRE(f.get_enum() == mapbox::vector_tile::geom_type::LINESTRING);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::feature_message::GEOMETRY);
    auto geometry_iter = f.get_packed_uint32();
    auto itr = geometry_iter.begin();
    const auto end_itr = geometry_iter.end();
    // MoveTo, ParameterInteger, ParameterInteger
    // LineTo, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger
    // MoveTo, ParameterInteger, ParameterInteger
    // LineTo, ParameterInteger, ParameterInteger
    // Therefore 4 commands + 10 parameters = 14
    REQUIRE(std::distance(itr, end_itr) == 14);
    // MoveTo(0,0)
    CHECK(*itr++ == ((1 << 3) | 1u)); // 9
    CHECK(*itr++ == 0);
    CHECK(*itr++ == 0);
    // LineTo(1,1)
    CHECK(*itr++ == ((2 << 3) | 2u)); 
    CHECK(*itr++ == 2);
    CHECK(*itr++ == 2);
    // LineTo(2,2)
    CHECK(*itr++ == 2);
    CHECK(*itr++ == 2);
    // MoveTo(5,5)
    CHECK(*itr++ == ((1 << 3) | 1u)); // 9
    CHECK(*itr++ == 6);
    CHECK(*itr++ == 6);
    // LineTo(0,0)
    CHECK(*itr++ == ((1 << 3) | 2u)); 
    CHECK(*itr++ == 9);
    CHECK(*itr++ == 9);
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

TEST_CASE("encode simple polygon feature") {
    mapbox::geometry::polygon<std::int64_t> p0;
    p0.emplace_back();
    p0.back().emplace_back(0,0);
    p0.back().emplace_back(0,10);
    p0.back().emplace_back(-10,10);
    p0.back().emplace_back(-10,0);
    p0.back().emplace_back(0,0);
    mapbox::geometry::property_map properties;
    properties.emplace("null", mapbox::geometry::null_value_t());
    properties.emplace("bool", false);
    properties.emplace("uint", std::numeric_limits<std::uint64_t>::min());
    properties.emplace("int", std::numeric_limits<std::int64_t>::min());
    properties.emplace("double", std::numeric_limits<double>::min());
    properties.emplace("string", std::string("fee"));
    std::experimental::optional<mapbox::geometry::identifier> id(static_cast<int64_t>(100));
    
    mapbox::geometry::feature<std::int64_t> feature = { p0, properties, id };

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
    REQUIRE(f.get_enum() == mapbox::vector_tile::geom_type::POLYGON);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::feature_message::GEOMETRY);
    auto geometry_iter = f.get_packed_uint32();
    auto itr = geometry_iter.begin();
    const auto end_itr = geometry_iter.end();
    // MoveTo, ParameterInteger, ParameterInteger
    // LineTo, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger
    // Close
    // 3 commands + 8 Params = 11 
    REQUIRE(std::distance(itr, end_itr) == 11);
    // MoveTo(0,0)
    CHECK(*itr++ == ((1 << 3) | 1u)); // 9
    CHECK(*itr++ == 0);
    CHECK(*itr++ == 0);
    // LineTo(0,10)
    CHECK(*itr++ == ((3 << 3) | 2u));
    CHECK(*itr++ == 0);
    CHECK(*itr++ == 20);
    // LineTo(-10,10)
    CHECK(*itr++ == 19);
    CHECK(*itr++ == 0);
    // LineTo(-10,0)
    CHECK(*itr++ == 0);
    CHECK(*itr++ == 19);
    // Close
    CHECK(*itr++ == 15);
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

TEST_CASE("encode simple multi polygon feature") {
    mapbox::geometry::multi_polygon<std::int64_t> mp;
    {
        mapbox::geometry::polygon<std::int64_t> p0;
        p0.emplace_back();
        p0.back().emplace_back(0,0);
        p0.back().emplace_back(0,10);
        p0.back().emplace_back(-10,10);
        p0.back().emplace_back(-10,0);
        p0.back().emplace_back(0,0);
        mapbox::geometry::linear_ring<std::int64_t> hole;
        hole.emplace_back(-7,7);
        hole.emplace_back(-3,7);
        hole.emplace_back(-3,3);
        hole.emplace_back(-7,3);
        hole.emplace_back(-7,7);
        p0.push_back(std::move(hole));
        mp.push_back(p0);
    }
    {
        mapbox::geometry::polygon<std::int64_t> p0;
        p0.emplace_back();
        p0.back().emplace_back(0,0);
        p0.back().emplace_back(0,10);
        p0.back().emplace_back(-10,10);
        p0.back().emplace_back(-10,0);
        p0.back().emplace_back(0,0);
        mapbox::geometry::linear_ring<std::int64_t> hole;
        hole.emplace_back(-7,7);
        hole.emplace_back(-3,7);
        hole.emplace_back(-3,3);
        hole.emplace_back(-7,3);
        hole.emplace_back(-7,7);
        p0.push_back(std::move(hole));
        mp.push_back(p0);
    }
    mapbox::geometry::property_map properties;
    properties.emplace("null", mapbox::geometry::null_value_t());
    properties.emplace("bool", false);
    properties.emplace("uint", std::numeric_limits<std::uint64_t>::min());
    properties.emplace("int", std::numeric_limits<std::int64_t>::min());
    properties.emplace("double", std::numeric_limits<double>::min());
    properties.emplace("string", std::string("fee"));
    std::experimental::optional<mapbox::geometry::identifier> id(static_cast<int64_t>(100));
    
    mapbox::geometry::feature<std::int64_t> feature = { mp, properties, id };

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
    REQUIRE(f.get_enum() == mapbox::vector_tile::geom_type::POLYGON);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::feature_message::GEOMETRY);
    auto geometry_iter = f.get_packed_uint32();
    auto itr = geometry_iter.begin();
    const auto end_itr = geometry_iter.end();
    // MoveTo, ParameterInteger, ParameterInteger
    // LineTo, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger
    // Close
    // MoveTo, ParameterInteger, ParameterInteger
    // LineTo, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger
    // Close
    // MoveTo, ParameterInteger, ParameterInteger
    // LineTo, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger
    // Close
    // MoveTo, ParameterInteger, ParameterInteger
    // LineTo, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger
    // Close
    // 12 commands + 32 Params = 44
    REQUIRE(std::distance(itr, end_itr) == 44);
    // MoveTo(0,0)
    CHECK(*itr++ == ((1 << 3) | 1u)); // 9
    CHECK(*itr++ == 0);
    CHECK(*itr++ == 0);
    // LineTo(0,10)
    CHECK(*itr++ == ((3 << 3) | 2u));
    CHECK(*itr++ == 0);
    CHECK(*itr++ == 20);
    // LineTo(-10,10)
    CHECK(*itr++ == 19);
    CHECK(*itr++ == 0);
    // LineTo(-10,0)
    CHECK(*itr++ == 0);
    CHECK(*itr++ == 19);
    // Close
    CHECK(*itr++ == 15);
    // Remember the cursor didn't move after the close
    // so it is at -10,0
    // MoveTo(-7,7)
    CHECK(*itr++ == ((1 << 3) | 1u)); // 9
    CHECK(*itr++ == 6);
    CHECK(*itr++ == 14);
    // LineTo(-3,7)
    CHECK(*itr++ == ((3 << 3) | 2u));
    CHECK(*itr++ == 8);
    CHECK(*itr++ == 0);
    // LineTo(-3,3)
    CHECK(*itr++ == 0);
    CHECK(*itr++ == 7);
    // LineTo(-7,3)
    CHECK(*itr++ == 7);
    CHECK(*itr++ == 0);
    // Close
    CHECK(*itr++ == 15);
    // Remember the cursor didn't move after the close
    // so it is at -7,3
    // MoveTo(0,0)
    CHECK(*itr++ == ((1 << 3) | 1u)); // 9
    CHECK(*itr++ == 14);
    CHECK(*itr++ == 5);
    // LineTo(0,10)
    CHECK(*itr++ == ((3 << 3) | 2u));
    CHECK(*itr++ == 0);
    CHECK(*itr++ == 20);
    // LineTo(-10,10)
    CHECK(*itr++ == 19);
    CHECK(*itr++ == 0);
    // LineTo(-10,0)
    CHECK(*itr++ == 0);
    CHECK(*itr++ == 19);
    // Close
    CHECK(*itr++ == 15);
    // Remember the cursor didn't move after the close
    // so it is at -10,0
    // MoveTo(-7,7)
    CHECK(*itr++ == ((1 << 3) | 1u)); // 9
    CHECK(*itr++ == 6);
    CHECK(*itr++ == 14);
    // LineTo(-3,7)
    CHECK(*itr++ == ((3 << 3) | 2u));
    CHECK(*itr++ == 8);
    CHECK(*itr++ == 0);
    // LineTo(-3,3)
    CHECK(*itr++ == 0);
    CHECK(*itr++ == 7);
    // LineTo(-7,3)
    CHECK(*itr++ == 7);
    CHECK(*itr++ == 0);
    // Close
    CHECK(*itr++ == 15);
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

TEST_CASE("encode geometry collection feature") {

    mapbox::geometry::point<std::int64_t> pt1(10,10);
    mapbox::geometry::point<std::int64_t> pt2(5,5);
    mapbox::geometry::geometry_collection<std::int64_t> gc;
    gc.emplace_back(pt1);
    gc.emplace_back(pt2);
    mapbox::geometry::property_map properties;
    properties.emplace("null", mapbox::geometry::null_value_t());
    properties.emplace("bool", true);
    properties.emplace("uint", static_cast<uint64_t>(1));
    properties.emplace("int", static_cast<int64_t>(2));
    properties.emplace("double", static_cast<double>(3.14));
    properties.emplace("string", std::string("foo"));
    std::experimental::optional<mapbox::geometry::identifier> id(static_cast<int64_t>(100));
    
    mapbox::geometry::feature<std::int64_t> feature = { gc, properties, id };

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
    {
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
    }
    {
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
        CHECK(*itr++ == 10);
        CHECK(*itr++ == 10);
        REQUIRE(f.next());
        REQUIRE(f.tag() == mapbox::vector_tile::feature_message::ID);
        CHECK(f.get_uint64() == 100);
        REQUIRE(f.next());
        REQUIRE(f.tag() == mapbox::vector_tile::feature_message::TAGS);
        auto tags_iter = f.get_packed_uint32();
        REQUIRE(std::distance(tags_iter.begin(), tags_iter.end()) == 10);
        REQUIRE_FALSE(f.next());
    }
    REQUIRE_FALSE(l.next());
}
