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

TEST_CASE("Read feature point")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/017/tile.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    REQUIRE(fm.size() == 1);
    REQUIRE(fm.end() != fm.find("hello"));
    auto fc = fm["hello"];
    REQUIRE(fc.size() == 1);
    auto f = fc[0];
    REQUIRE(f.id.is<std::uint64_t>());
    CHECK(f.id.get<std::uint64_t>() == 1);
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

TEST_CASE("Read feature multipoint")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/020/tile.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    REQUIRE(fm.size() == 1);
    REQUIRE(fm.end() != fm.find("hello"));
    auto fc = fm["hello"];
    REQUIRE(fc.size() == 1);
    auto f = fc[0];
    REQUIRE(f.id.is<std::uint64_t>());
    CHECK(f.id.get<std::uint64_t>() == 1);
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

TEST_CASE("Read feature linestring")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/018/tile.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    REQUIRE(fm.size() == 1);
    REQUIRE(fm.end() != fm.find("hello"));
    auto fc = fm["hello"];
    REQUIRE(fc.size() == 1);
    auto f = fc[0];
    REQUIRE(f.id.is<std::uint64_t>());
    CHECK(f.id.get<std::uint64_t>() == 1);
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

TEST_CASE("Read feature multilinestring")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/021/tile.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    REQUIRE(fm.size() == 1);
    REQUIRE(fm.end() != fm.find("hello"));
    auto fc = fm["hello"];
    REQUIRE(fc.size() == 1);
    auto f = fc[0];
    REQUIRE(f.id.is<std::uint64_t>());
    CHECK(f.id.get<std::uint64_t>() == 1);
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

TEST_CASE("Read feature polygon")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/019/tile.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    REQUIRE(fm.size() == 1);
    REQUIRE(fm.end() != fm.find("hello"));
    auto fc = fm["hello"];
    REQUIRE(fc.size() == 1);
    auto f = fc[0];
    REQUIRE(f.id.is<std::uint64_t>());
    CHECK(f.id.get<std::uint64_t>() == 1);
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

TEST_CASE("Read feature multipolygon")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/022/tile.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    REQUIRE(fm.size() == 1);
    REQUIRE(fm.end() != fm.find("hello"));
    auto fc = fm["hello"];
    REQUIRE(fc.size() == 1);
    auto f = fc[0];
    REQUIRE(f.id.is<std::uint64_t>());
    CHECK(f.id.get<std::uint64_t>() == 1);
    REQUIRE(f.properties.size() == 1);
    REQUIRE(f.properties.end() != f.properties.find("hello"));
    auto val = f.properties["hello"];
    REQUIRE(val.is<std::string>());
    CHECK(val.get<std::string>() == "world");
    REQUIRE(f.geometry.is<mapbox::geometry::multi_polygon<std::int64_t>>());
    auto mpoly = f.geometry.get<mapbox::geometry::multi_polygon<std::int64_t>>();
    REQUIRE(mpoly.size() == 2);
    REQUIRE(mpoly[0].size() == 1);
    REQUIRE(mpoly[0][0].size() == 5);
    CHECK(mpoly[0][0][0].x == 0);
    CHECK(mpoly[0][0][0].y == 0);
    CHECK(mpoly[0][0][1].x == 10);
    CHECK(mpoly[0][0][1].y == 0);
    CHECK(mpoly[0][0][2].x == 10);
    CHECK(mpoly[0][0][2].y == 10);
    CHECK(mpoly[0][0][3].x == 0);
    CHECK(mpoly[0][0][3].y == 10);
    CHECK(mpoly[0][0][4].x == 0);
    CHECK(mpoly[0][0][4].y == 0);
}

TEST_CASE("Read invalid: missing geometry type field, results in feature collection and no layer")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/003/tile.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    CHECK(fm.empty());
}

TEST_CASE("Read invalid: missing geometry field, results in an exception from vtzero")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/004/tile.mvt");
    CHECK_THROWS_WITH(mapbox::vector_tile::decode_tile<std::int64_t>(buffer), "Missing geometry field in feature (spec 4.2)");
}

TEST_CASE("Read invalid: the tags array has only a single tag, where multiples of two are required, results in an exception from vtzero")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/005/tile.mvt");
    CHECK_THROWS_WITH(mapbox::vector_tile::decode_tile<std::int64_t>(buffer), "unpaired property key/value indexes (spec 4.4)");
}

TEST_CASE("Read invalid: invalid geometry type enum value, results in an exception from vtzero")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/006/tile.mvt");
    CHECK_THROWS_WITH(mapbox::vector_tile::decode_tile<std::int64_t>(buffer), "Unknown geometry type (spec 4.3.4)");
}
