#include "catch.hpp"

#include <mapbox/geometry.hpp>
#include <mapbox/vector_tile/encode_geometry.hpp>

// protozero
#include <protozero/pbf_writer.hpp>
#include <protozero/pbf_reader.hpp>

// std
#include <limits>

//
// Unit tests for geometry encoding of linestrings
//

TEST_CASE("encode pbf simple line_string") {
    mapbox::geometry::line_string<std::int64_t> line;
    line.emplace_back(10,10);
    line.emplace_back(20,20);
    line.emplace_back(30,30);
    
    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE(mapbox::vector_tile::encode_geometry(line, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::LINESTRING);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::GEOMETRY);
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
}

TEST_CASE("encode pbf overlapping line_string") {
    mapbox::geometry::line_string<std::int64_t> line;
    line.emplace_back(10,10);
    line.emplace_back(20,20);
    line.emplace_back(10,10);
    
    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE(mapbox::vector_tile::encode_geometry(line, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::LINESTRING);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::GEOMETRY);
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
    // LineTo(10,10)
    CHECK(*itr++ == 19);
    CHECK(*itr++ == 19);
}

TEST_CASE("encode pbf line_string with repeated points") {
    mapbox::geometry::line_string<std::int64_t> line;
    line.emplace_back(10,10);
    line.emplace_back(10,10);
    line.emplace_back(10,10);
    line.emplace_back(20,20);
    line.emplace_back(20,20);
    line.emplace_back(20,20);
    line.emplace_back(30,30);
    line.emplace_back(30,30);
    line.emplace_back(30,30);
    
    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE(mapbox::vector_tile::encode_geometry(line, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::LINESTRING);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::GEOMETRY);
    auto geometry_iter = f.get_packed_uint32();
    auto itr = geometry_iter.begin();
    const auto end_itr = geometry_iter.end();
    
    // All of the repeated points should be removed resulting in the following:
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
}

TEST_CASE("encode pbf degenerate line_string") {
    mapbox::geometry::line_string<std::int64_t> line;
    line.emplace_back(10,10);
    
    // since the line is degenerate the whole line should be culled during encoding
    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE_FALSE(mapbox::vector_tile::encode_geometry(line, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::LINESTRING);
    REQUIRE_FALSE(f.next());
}

TEST_CASE("encode pbf degenerate line_string all repeated points") {
    mapbox::geometry::line_string<std::int64_t> line;
    line.emplace_back(10,10);
    line.emplace_back(10,10);
    line.emplace_back(10,10);
    line.emplace_back(10,10);
    
    // since the line is degenerate the whole line should be culled during encoding
    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE_FALSE(mapbox::vector_tile::encode_geometry(line, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::LINESTRING);
    REQUIRE_FALSE(f.next());
}

TEST_CASE("encode pbf incredibly large segments") {
    // This is a test case added that is known to completely break the logic
    // within the encoder. 
    std::int64_t val = std::numeric_limits<std::int64_t>::max();
    mapbox::geometry::line_string<std::int64_t> line;
    line.emplace_back(0,0);
    line.emplace_back(val,val);
    line.emplace_back(0,0);

    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE(mapbox::vector_tile::encode_geometry(line, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::LINESTRING);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::GEOMETRY);
    auto geometry_iter = f.get_packed_uint32();
    auto itr = geometry_iter.begin();
    const auto end_itr = geometry_iter.end();
    
    // MoveTo, ParameterInteger, ParameterInteger
    // LineTo, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger
    // Therefore 2 commands + 6 parameters = 8
    REQUIRE(std::distance(itr, end_itr) == 8);
    // MoveTo(10,10)
    CHECK(*itr++ == ((1 << 3) | 1u)); // 9
    CHECK(*itr++ == 0);
    CHECK(*itr++ == 0);
    // LineTo(0,0)
    CHECK(*itr++ == ((2 << 3) | 2u)); 
    CHECK(*itr++ == 1);
    CHECK(*itr++ == 1);
    // LineTo(1,1)
    CHECK(*itr++ == 2);
    CHECK(*itr++ == 2);
}

TEST_CASE("encode pbf simple multi_line_string") {
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

    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE(mapbox::vector_tile::encode_geometry(g, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::LINESTRING);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::GEOMETRY);
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
}

TEST_CASE("encode pbf multi_line_string with repeated points")
{
    mapbox::geometry::multi_line_string<std::int64_t> g;
    mapbox::geometry::line_string<std::int64_t> l1;
    l1.emplace_back(0,0);
    l1.emplace_back(0,0);
    l1.emplace_back(0,0);
    l1.emplace_back(1,1);
    l1.emplace_back(1,1);
    l1.emplace_back(1,1);
    l1.emplace_back(2,2);
    l1.emplace_back(2,2);
    l1.emplace_back(2,2);
    g.push_back(std::move(l1));
    mapbox::geometry::line_string<std::int64_t> l2;
    l2.emplace_back(5,5);
    l2.emplace_back(5,5);
    l2.emplace_back(5,5);
    l2.emplace_back(0,0);
    l2.emplace_back(0,0);
    l2.emplace_back(0,0);
    g.push_back(std::move(l2));

    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE(mapbox::vector_tile::encode_geometry(g, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::LINESTRING);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::GEOMETRY);
    auto geometry_iter = f.get_packed_uint32();
    auto itr = geometry_iter.begin();
    const auto end_itr = geometry_iter.end();
    
    // Repeated commands should be removed points should be as follows:
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
}

TEST_CASE("encode pbf multi_line_string with two degenerate linestrings")
{
    mapbox::geometry::multi_line_string<std::int64_t> g;
    mapbox::geometry::line_string<std::int64_t> l1;
    l1.emplace_back(0,0);
    g.push_back(std::move(l1));
    mapbox::geometry::line_string<std::int64_t> l2;
    l2.emplace_back(5,0);
    l2.emplace_back(5,0);
    l2.emplace_back(5,0);
    g.push_back(std::move(l2));
    mapbox::geometry::line_string<std::int64_t> l3;
    l3.emplace_back(5,5);
    l3.emplace_back(0,0);
    g.push_back(std::move(l3));

    // Should remove first line string as it does not have enough points
    // and second linestring should be removed because it only has repeated
    // points and therefore is too small
    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE(mapbox::vector_tile::encode_geometry(g, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::LINESTRING);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::GEOMETRY);
    auto geometry_iter = f.get_packed_uint32();
    auto itr = geometry_iter.begin();
    const auto end_itr = geometry_iter.end();
    
    // MoveTo, ParameterInteger, ParameterInteger
    // LineTo, ParameterInteger, ParameterInteger
    // Therefore 2 commands + 4 parameters = 6
    REQUIRE(std::distance(itr, end_itr) == 6);
    // MoveTo(5,5)
    CHECK(*itr++ == ((1 << 3) | 1u)); // 9
    CHECK(*itr++ == 10);
    CHECK(*itr++ == 10);
    // LineTo(0,0)
    CHECK(*itr++ == ((1 << 3) | 2u)); 
    CHECK(*itr++ == 9);
    CHECK(*itr++ == 9);
}
