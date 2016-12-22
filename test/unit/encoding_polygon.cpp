#include "catch.hpp"

#include <mapbox/geometry.hpp>
#include <mapbox/vector_tile/encode_geometry.hpp>

// protozero
#include <protozero/pbf_writer.hpp>
#include <protozero/pbf_reader.hpp>

//
// Unit tests for geometry encoding of polygons
//

TEST_CASE("encoding simple polygon") {
    mapbox::geometry::polygon<std::int64_t> p0;
    p0.emplace_back();
    p0.back().emplace_back(0,0);
    p0.back().emplace_back(0,10);
    p0.back().emplace_back(-10,10);
    p0.back().emplace_back(-10,0);
    p0.back().emplace_back(0,0);
    
    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE(mapbox::vector_tile::encode_geometry(p0, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::POLYGON);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::GEOMETRY);
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
}

TEST_CASE("encoding simple polygon with hole") {
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

    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE(mapbox::vector_tile::encode_geometry(p0, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::POLYGON);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::GEOMETRY);
    auto geometry_iter = f.get_packed_uint32();
    auto itr = geometry_iter.begin();
    const auto end_itr = geometry_iter.end();
    
    // MoveTo, ParameterInteger, ParameterInteger
    // LineTo, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger
    // Close
    // MoveTo, ParameterInteger, ParameterInteger
    // LineTo, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger, ParameterInteger
    // Close
    // 6 commands + 16 Params = 22 
    REQUIRE(std::distance(itr, end_itr) == 22);
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
}

TEST_CASE("encoding empty polygon") {
    mapbox::geometry::polygon<std::int64_t> p;
    
    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE_FALSE(mapbox::vector_tile::encode_geometry(p, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::POLYGON);
    REQUIRE_FALSE(f.next());
}

TEST_CASE("encoding multi polygons with holes") {
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
    // yeah so its the same polygon -- haters gonna hate.
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

    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE(mapbox::vector_tile::encode_geometry(mp, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::POLYGON);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::GEOMETRY);
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
}

TEST_CASE("encoding empty multi polygon") {
    mapbox::geometry::multi_polygon<std::int64_t> mp;
    
    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE_FALSE(mapbox::vector_tile::encode_geometry(mp, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::POLYGON);
    REQUIRE_FALSE(f.next());
}

TEST_CASE("encoding polygon with degenerate exterior ring full of repeated points") {
    mapbox::geometry::polygon<std::int64_t> p0;
    p0.emplace_back();
    // invalid exterior ring
    p0.back().emplace_back(0,0);
    p0.back().emplace_back(0,10);
    p0.back().emplace_back(0,10);
    p0.back().emplace_back(0,10);
    p0.back().emplace_back(0,10);
    p0.back().emplace_back(0,10);
    p0.back().emplace_back(0,10);
    p0.back().emplace_back(0,10);

    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE_FALSE(mapbox::vector_tile::encode_geometry(p0, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::POLYGON);
    REQUIRE_FALSE(f.next());
}

TEST_CASE("encoding polygon with degenerate exterior ring") {
    mapbox::geometry::polygon<std::int64_t> p0;
    p0.emplace_back();
    // invalid exterior ring
    p0.back().emplace_back(0,0);
    p0.back().emplace_back(0,10);

    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE_FALSE(mapbox::vector_tile::encode_geometry(p0, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::POLYGON);
    REQUIRE_FALSE(f.next());
}

TEST_CASE("encoding polygon with degenerate exterior ring and interior ring") {
    mapbox::geometry::polygon<std::int64_t> p0;
    p0.emplace_back();
    // invalid exterior ring
    p0.back().emplace_back(0,0);
    p0.back().emplace_back(0,10);
    // invalid interior ring -- is counter clockwise
    mapbox::geometry::linear_ring<std::int64_t> hole;
    hole.emplace_back(-7,7);
    hole.emplace_back(-3,7);
    hole.emplace_back(-3,3);
    hole.emplace_back(-7,3);
    hole.emplace_back(-7,7);
    p0.push_back(std::move(hole));

    // encoder should cull the exterior invalid ring, which triggers
    // the entire polygon to be culled.
    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE_FALSE(mapbox::vector_tile::encode_geometry(p0, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::POLYGON);
    REQUIRE_FALSE(f.next());
}

TEST_CASE("encoding polygon with valid exterior ring but degenerate interior ring") {
    mapbox::geometry::polygon<std::int64_t> p0;
    p0.emplace_back();
    p0.back().emplace_back(0,0);
    p0.back().emplace_back(0,10);
    p0.back().emplace_back(-10,10);
    p0.back().emplace_back(-10,0);
    p0.back().emplace_back(0,0);
    // invalid interior ring
    mapbox::geometry::linear_ring<std::int64_t> hole;
    hole.emplace_back(-7,7);
    hole.emplace_back(-3,7);
    p0.push_back(std::move(hole));

    std::string feature_str;
    protozero::pbf_writer feature_writer(feature_str);
    REQUIRE(mapbox::vector_tile::encode_geometry(p0, feature_writer));
    
    protozero::pbf_reader f(feature_str);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::TYPE);
    REQUIRE(f.get_enum() == mapbox::vector_tile::GeomType::POLYGON);
    REQUIRE(f.next());
    REQUIRE(f.tag() == mapbox::vector_tile::FeatureType::GEOMETRY);
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
}

