#include <mapbox/vector_tile.hpp>
#include <mapbox/vector_tile/version.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#include <catch.hpp>

static std::string open_tile(std::string const& path)
{
    std::ifstream stream(path.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!stream.is_open())
    {
        throw std::runtime_error("could not open: '" + path + "'");
    }
    std::string message(std::istreambuf_iterator<char>(stream.rdbuf()), (std::istreambuf_iterator<char>()));
    stream.close();
    return message;
}

TEST_CASE("Read Feature-single-point.mvt")
{
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

TEST_CASE("Read Feature-single-multipoint.mvt")
{
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
    REQUIRE(mp.size() == 2);
    CHECK(mp[0].x == 5);
    CHECK(mp[0].y == 7);
    CHECK(mp[1].x == 3);
    CHECK(mp[1].y == 2);
}

TEST_CASE("Read Feature-single-linestring.mvt")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/valid/Feature-single-linestring.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    REQUIRE(fm.size() == 1);
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
    REQUIRE(f.geometry.is<mapbox::geometry::line_string<std::int64_t>>());
    auto ls = f.geometry.get<mapbox::geometry::line_string<std::int64_t>>();
    REQUIRE(ls.size() == 3);
    CHECK(ls[0].x == 2);
    CHECK(ls[0].y == 2);
    CHECK(ls[1].x == 2);
    CHECK(ls[1].y == 10);
    CHECK(ls[2].x == 10);
    CHECK(ls[2].y == 10);
}

TEST_CASE("Read Feature-single-multilinestring.mvt")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/valid/Feature-single-multilinestring.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    REQUIRE(fm.size() == 1);
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
    REQUIRE(f.geometry.is<mapbox::geometry::multi_line_string<std::int64_t>>());
    auto mls = f.geometry.get<mapbox::geometry::multi_line_string<std::int64_t>>();
    REQUIRE(mls.size() == 2);
    REQUIRE(mls[0].size() == 3);
    CHECK(mls[0][0].x == 2);
    CHECK(mls[0][0].y == 2);
    CHECK(mls[0][1].x == 2);
    CHECK(mls[0][1].y == 10);
    CHECK(mls[0][2].x == 10);
    CHECK(mls[0][2].y == 10);
    REQUIRE(mls[1].size() == 2);
    CHECK(mls[1][0].x == 1);
    CHECK(mls[1][0].y == 1);
    CHECK(mls[1][1].x == 3);
    CHECK(mls[1][1].y == 5);
}

TEST_CASE("Read Feature-single-polygon.mvt")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/valid/Feature-single-polygon.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    REQUIRE(fm.size() == 1);
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
    REQUIRE(f.geometry.is<mapbox::geometry::polygon<std::int64_t>>());
    auto poly = f.geometry.get<mapbox::geometry::polygon<std::int64_t>>();
    REQUIRE(poly.size() == 1);
    REQUIRE(poly[0].size() == 4);
    CHECK(poly[0][0].x == 3);
    CHECK(poly[0][0].y == 6);
    CHECK(poly[0][1].x == 8);
    CHECK(poly[0][1].y == 12);
    CHECK(poly[0][2].x == 20);
    CHECK(poly[0][2].y == 34);
}

/*
TEST_CASE( "Read Feature-single-multipolygon.mvt" ) {
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/valid/Feature-single-multipolygon.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    REQUIRE(fm.size() == 1);
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
    REQUIRE(f.geometry.is<mapbox::geometry::multi_polygon<std::int64_t>>());
    auto mpoly = f.geometry.get<mapbox::geometry::multi_polygon<std::int64_t>>();
    REQUIRE(mpoly.size() == 2);
    REQUIRE(mpoly[0].size() == 4);
    CHECK(mpoly[0][0][0].x == 3);
    CHECK(mpoly[0][0][0].y == 6);
    CHECK(mpoly[0][0][1].x == 8);
    CHECK(mpoly[0][0][1].y == 12);
    CHECK(mpoly[0][0][2].x == 20);
    CHECK(mpoly[0][0][2].y == 34);
}*/
