#include "catch.hpp"

#include <mapbox/geometry.hpp>
#include <mapbox/vector_tile/encode_geometry.hpp>

// protozero
#include <protozero/pbf_writer.hpp>
#include <protozero/pbf_reader.hpp>

// std
#include <limits>

//
// Unit tests for geometry encoding of points
//

TEST_CASE("encode pbf simple point")
{
    mapbox::geometry::point<std::int64_t> point(10,10);
    
    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE(mapbox::vector_tile::encode_geometry(point, feature_writer));
    
    protozero::pbf_reader f(feature_str);
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
}

TEST_CASE("encode pbf simple negative point")
{
    mapbox::geometry::point<std::int64_t> point(-10,-10);
    
    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE(mapbox::vector_tile::encode_geometry(point, feature_writer));
    
    protozero::pbf_reader f(feature_str);
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
    REQUIRE( std::distance(itr, end_itr) == 3);
    // MoveTo(10,10)
    CHECK(*itr++ == ((1 << 3) | 1u)); // 9
    CHECK(*itr++ == 19);
    CHECK(*itr++ == 19);
}

TEST_CASE("encode pbf simple multi point")
{
    mapbox::geometry::multi_point<std::int64_t> mp;
    mp.emplace_back(10,10);
    mp.emplace_back(20,20);
    mp.emplace_back(30,30);
    
    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE(mapbox::vector_tile::encode_geometry(mp, feature_writer));
    
    protozero::pbf_reader f(feature_str);
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
}

TEST_CASE("encode pbf multi point with repeated points")
{
    mapbox::geometry::multi_point<std::int64_t> mp;
    mp.emplace_back(10,10);
    mp.emplace_back(10,10);
    mp.emplace_back(20,20);
    
    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE(mapbox::vector_tile::encode_geometry(mp, feature_writer));
    
    protozero::pbf_reader f(feature_str);
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
    CHECK(*itr++ == 0);
    CHECK(*itr++ == 0);
    // MoveTo(20,20)
    CHECK(*itr++ == 20);
    CHECK(*itr++ == 20);
}

TEST_CASE("encode pbf empty multi point geometry")
{
    mapbox::geometry::multi_point<std::int64_t> mp;
    
    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE_FALSE(mapbox::vector_tile::encode_geometry(mp, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::feature_message::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::geom_type::POINT);
    REQUIRE_FALSE(f.next());
}

