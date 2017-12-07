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

TEST_CASE("Read: UNKNOWN geometry type, results in no features")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/016/tile.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    CHECK(fm.empty());
}

TEST_CASE("Read: missing geometry type field, results in feature collection and no layer")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/003/tile.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    CHECK(fm.empty());
}

TEST_CASE("Read: missing layer extent is assigned the default value")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/009/tile.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    REQUIRE(fm.size() == 1);
    REQUIRE(fm.end() != fm.find("hello"));
    auto fc = fm["hello"];
    REQUIRE(fc.size() == 1);
    auto f = fc[0];
    REQUIRE(f.geometry.is<mapbox::geometry::point<std::int64_t>>());
    auto pt = f.geometry.get<mapbox::geometry::point<std::int64_t>>();
    CHECK(pt.x == 25);
    CHECK(pt.y == 17);
    // just creating a point is a check that we are handling with a default extent,
    // but we'll want to assert on extent somewhere in the future
}

TEST_CASE("Read: A Layer value property is listed as 'string' but encoded as std::int64_t, results in a skip and no properties on feature")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/010/tile.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    REQUIRE(fm.size() == 1);
    REQUIRE(fm.end() != fm.find("hello"));
    auto fc = fm["hello"];
    REQUIRE(fc.size() == 1);
    auto f = fc[0];
    CHECK(f.properties.empty());
}

TEST_CASE("Read: two layers with the same name value, but only the first layer added is kept")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/015/tile.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    REQUIRE(fm.size() == 1);
    // right now we default to the std::map::emplace functionality, which will automatically
    // remove an element where the key currently exists - meaning the first layer added
    // will be the final layer. Is this what we want? Can we override the feature
    // collection map to throw an exception? Something to benchmark and think about.
    REQUIRE(fm.size() == 1);
    REQUIRE(fm.end() != fm.find("hello"));
    auto fc = fm["hello"];
    REQUIRE(fc.size() == 1);
    auto f = fc[0];
    REQUIRE(f.properties.size() == 1);
    auto val = f.properties["name"];
    CHECK(val.is<std::string>());
    CHECK(val.get<std::string>() == "layer-one");
}

TEST_CASE("Read: missing layer version property, defaults to a specific version and properly parses")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/024/tile.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    CHECK(!fm.empty());
    // not exactly sure how to test that this one works other than making sure decode_tile actually works
}

TEST_CASE("Read: layer has no features, results in an empty feature map")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/025/tile.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    CHECK(fm.empty());
}

TEST_CASE("Read: has an extra Value type called 'my_value' which is not handled in the property map")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/026/tile.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    REQUIRE(fm.size() == 1);
    auto fc = fm["hello"];
    REQUIRE(fc.empty()); // my_value does not make it through
}

TEST_CASE("Read: all valid property value types")
{
    std::string buffer = open_tile("test/mvt-fixtures/fixtures/038/tile.mvt");
    auto fm = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
    REQUIRE(fm.size() == 1);
    REQUIRE(fm.end() != fm.find("hello"));
    auto fc = fm["hello"];
    REQUIRE(fc.size() == 1);
    auto f = fc[0];
    REQUIRE(f.properties.size() == 7);

    REQUIRE(f.properties.end() != f.properties.find("string_value"));
    auto string_value = f.properties["string_value"];
    CHECK(string_value.is<std::string>());
    CHECK(string_value.get<std::string>() == "ello");

    REQUIRE(f.properties.end() != f.properties.find("bool_value"));
    auto bool_value = f.properties["bool_value"];
    CHECK(bool_value.is<bool>());
    CHECK(bool_value.get<bool>() == true);

    REQUIRE(f.properties.end() != f.properties.find("int_value"));
    auto int_value = f.properties["int_value"];
    CHECK(int_value.is<std::int64_t>());
    CHECK(int_value.get<std::int64_t>() == 6);

    REQUIRE(f.properties.end() != f.properties.find("double_value"));
    auto double_value = f.properties["double_value"];
    CHECK(double_value.is<double>());
    CHECK(double_value.get<double>() == 1.23);

    REQUIRE(f.properties.end() != f.properties.find("float_value"));
    auto float_value = f.properties["float_value"];
    CHECK(float_value.is<double>());
    CHECK(float_value.get<double>() == Approx(3.1));

    REQUIRE(f.properties.end() != f.properties.find("sint_value"));
    auto sint_value = f.properties["sint_value"];
    CHECK(sint_value.is<std::int64_t>());
    CHECK(sint_value.get<std::int64_t>() == -87948);

    REQUIRE(f.properties.end() != f.properties.find("uint_value"));
    auto uint_value = f.properties["uint_value"];
    CHECK(uint_value.is<std::uint64_t>());
    CHECK(uint_value.get<std::uint64_t>() == 87948);
}

TEST_CASE("Read: vtzero exceptions")
{
    std::string buffer004 = open_tile("test/mvt-fixtures/fixtures/004/tile.mvt");
    CHECK_THROWS(mapbox::vector_tile::decode_tile<std::int64_t>(buffer004));
    std::string buffer005 = open_tile("test/mvt-fixtures/fixtures/005/tile.mvt");
    CHECK_THROWS(mapbox::vector_tile::decode_tile<std::int64_t>(buffer005));
    std::string buffer006 = open_tile("test/mvt-fixtures/fixtures/006/tile.mvt");
    CHECK_THROWS(mapbox::vector_tile::decode_tile<std::int64_t>(buffer006));
    std::string buffer007 = open_tile("test/mvt-fixtures/fixtures/007/tile.mvt");
    CHECK_THROWS(mapbox::vector_tile::decode_tile<std::int64_t>(buffer007));
    std::string buffer008 = open_tile("test/mvt-fixtures/fixtures/008/tile.mvt");
    CHECK_THROWS(mapbox::vector_tile::decode_tile<std::int64_t>(buffer008));
    std::string buffer011 = open_tile("test/mvt-fixtures/fixtures/011/tile.mvt");
    CHECK_THROWS(mapbox::vector_tile::decode_tile<std::int64_t>(buffer011));
    std::string buffer012 = open_tile("test/mvt-fixtures/fixtures/012/tile.mvt");
    CHECK_THROWS(mapbox::vector_tile::decode_tile<std::int64_t>(buffer012));
    std::string buffer013 = open_tile("test/mvt-fixtures/fixtures/013/tile.mvt");
    CHECK_THROWS(mapbox::vector_tile::decode_tile<std::int64_t>(buffer013));
    std::string buffer014 = open_tile("test/mvt-fixtures/fixtures/014/tile.mvt");
    CHECK_THROWS(mapbox::vector_tile::decode_tile<std::int64_t>(buffer014));
    std::string buffer023 = open_tile("test/mvt-fixtures/fixtures/023/tile.mvt");
    CHECK_THROWS(mapbox::vector_tile::decode_tile<std::int64_t>(buffer023));
    std::string buffer030 = open_tile("test/mvt-fixtures/fixtures/030/tile.mvt");
    CHECK_THROWS(mapbox::vector_tile::decode_tile<std::int64_t>(buffer030));
    std::string buffer040 = open_tile("test/mvt-fixtures/fixtures/040/tile.mvt");
    CHECK_THROWS(mapbox::vector_tile::decode_tile<std::int64_t>(buffer040));
    std::string buffer041 = open_tile("test/mvt-fixtures/fixtures/041/tile.mvt");
    CHECK_THROWS(mapbox::vector_tile::decode_tile<std::int64_t>(buffer041));
    std::string buffer042 = open_tile("test/mvt-fixtures/fixtures/042/tile.mvt");
    CHECK_THROWS(mapbox::vector_tile::decode_tile<std::int64_t>(buffer042));
    std::string buffer044 = open_tile("test/mvt-fixtures/fixtures/044/tile.mvt");
    CHECK_THROWS(mapbox::vector_tile::decode_tile<std::int64_t>(buffer044));
}
