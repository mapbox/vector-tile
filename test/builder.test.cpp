#include <mapbox/vector_tile/builder.hpp>
#include <mapbox/vector_tile.hpp>
#include <catch.hpp>

TEST_CASE("Encode feature")
{
    vtzero::tile_builder tbuilder;
    vtzero::layer_builder lbuilder{tbuilder, "my_layer_name", 2, 4097};
    mapbox::feature::property_map prop_in;
    mapbox::feature::property_map prop_out;

    prop_in["a_string"] = mapbox::feature::value(std::string("some_string")); 
    prop_in["a_bool"] = mapbox::feature::value(true); 
    prop_in["a_int"] = mapbox::feature::value(std::int64_t(-4)); 
    prop_in["a_uint"] = mapbox::feature::value(std::uint64_t(12)); 
    prop_in["a_double"] = mapbox::feature::value(double(5.0)); 
    prop_in["a_null"] = mapbox::feature::value();
    
    SECTION("point")
    {
        mapbox::geometry::point<std::int64_t> pt_in { 25, 17 };
        mapbox::geometry::point<std::int64_t> pt_out;
        mapbox::geometry::geometry<std::int64_t> geom_in(pt_in);
        
        SECTION("id int64_t")
        {
            mapbox::feature::identifier id(std::int64_t(10));
            mapbox::feature::feature<std::int64_t> feat { geom_in, prop_in, id };
            mapbox::vector_tile::encode_feature(lbuilder, feat);

            std::string buffer = tbuilder.serialize();
            auto const fc_map = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
            REQUIRE(fc_map.size() == 1);
            for (auto const& l : fc_map) 
            {
                REQUIRE(l.first == "my_layer_name");
                REQUIRE(l.second.size() == 1);
                for (auto const& f : l.second)
                {
                    REQUIRE(f.id.is<std::uint64_t>());
                    CHECK(f.id.get<std::uint64_t>() == 10);
                    REQUIRE(f.geometry.is<mapbox::geometry::point<std::int64_t>>());
                    prop_out = f.properties;
                    pt_out = f.geometry.get<mapbox::geometry::point<std::int64_t>>();
                }
            }
        }
        
        SECTION("id negative int64_t")
        {
            mapbox::feature::identifier id(std::int64_t(-10));
            mapbox::feature::feature<std::int64_t> feat { geom_in, prop_in, id };
            mapbox::vector_tile::encode_feature(lbuilder, feat);

            std::string buffer = tbuilder.serialize();
            auto const fc_map = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
            REQUIRE(fc_map.size() == 1);
            for (auto const& l : fc_map) 
            {
                REQUIRE(l.first == "my_layer_name");
                REQUIRE(l.second.size() == 1);
                for (auto const& f : l.second)
                {
                    REQUIRE(f.id.is<mapbox::feature::null_value_t>());
                    REQUIRE(f.geometry.is<mapbox::geometry::point<std::int64_t>>());
                    prop_out = f.properties;
                    pt_out = f.geometry.get<mapbox::geometry::point<std::int64_t>>();
                }
            }
        }
        
        SECTION("id uint64_t")
        {
            mapbox::feature::identifier id(std::uint64_t(10));
            mapbox::feature::feature<std::int64_t> feat { geom_in, prop_in, id };
            mapbox::vector_tile::encode_feature(lbuilder, feat);

            std::string buffer = tbuilder.serialize();
            auto const fc_map = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
            REQUIRE(fc_map.size() == 1);
            for (auto const& l : fc_map) 
            {
                REQUIRE(l.first == "my_layer_name");
                REQUIRE(l.second.size() == 1);
                for (auto const& f : l.second)
                {
                    REQUIRE(f.id.is<std::uint64_t>());
                    CHECK(f.id.get<std::uint64_t>() == 10);
                    REQUIRE(f.geometry.is<mapbox::geometry::point<std::int64_t>>());
                    prop_out = f.properties;
                    pt_out = f.geometry.get<mapbox::geometry::point<std::int64_t>>();
                }
            }
        }
        
        SECTION("id double")
        {
            mapbox::feature::identifier id(double(10.0));
            mapbox::feature::feature<std::int64_t> feat { geom_in, prop_in, id };
            mapbox::vector_tile::encode_feature(lbuilder, feat);

            std::string buffer = tbuilder.serialize();
            auto const fc_map = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
            REQUIRE(fc_map.size() == 1);
            for (auto const& l : fc_map) 
            {
                REQUIRE(l.first == "my_layer_name");
                REQUIRE(l.second.size() == 1);
                for (auto const& f : l.second)
                {
                    REQUIRE(f.id.is<mapbox::feature::null_value_t>());
                    REQUIRE(f.geometry.is<mapbox::geometry::point<std::int64_t>>());
                    prop_out = f.properties;
                    pt_out = f.geometry.get<mapbox::geometry::point<std::int64_t>>();
                }
            }
        }

        SECTION("id null value")
        {
            mapbox::feature::identifier id;
            mapbox::feature::feature<std::int64_t> feat { geom_in, prop_in, id };
            mapbox::vector_tile::encode_feature(lbuilder, feat);

            std::string buffer = tbuilder.serialize();
            auto const fc_map = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
            REQUIRE(fc_map.size() == 1);
            for (auto const& l : fc_map) 
            {
                REQUIRE(l.first == "my_layer_name");
                REQUIRE(l.second.size() == 1);
                for (auto const& f : l.second)
                {
                    REQUIRE(f.id.is<mapbox::feature::null_value_t>());
                    REQUIRE(f.geometry.is<mapbox::geometry::point<std::int64_t>>());
                    prop_out = f.properties;
                    pt_out = f.geometry.get<mapbox::geometry::point<std::int64_t>>();
                }
            }
        }

        CHECK(pt_out.x == 25);
        CHECK(pt_out.y == 17);
    }

    SECTION("multi point")
    {
        mapbox::geometry::multi_point<std::int64_t> mp_in;
        mp_in.emplace_back(25, 17);
        mp_in.emplace_back(15, 12);
        mapbox::geometry::geometry<std::int64_t> geom_in(mp_in);
        mapbox::feature::identifier id;
        mapbox::feature::feature<std::int64_t> feat { geom_in, prop_in, id };
        mapbox::vector_tile::encode_feature(lbuilder, feat);
        std::string buffer = tbuilder.serialize();
        auto const fc_map = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
        REQUIRE(fc_map.size() == 1);
        for (auto const& l : fc_map) 
        {
            REQUIRE(l.first == "my_layer_name");
            REQUIRE(l.second.size() == 1);
            for (auto const& f : l.second)
            {
                REQUIRE(f.id.is<mapbox::feature::null_value_t>());
                REQUIRE(f.geometry.is<mapbox::geometry::multi_point<std::int64_t>>());
                prop_out = f.properties;
                auto mp_out = f.geometry.get<mapbox::geometry::multi_point<std::int64_t>>();
                REQUIRE(mp_out.size() == 2);
                CHECK(mp_out[0].x == 25);
                CHECK(mp_out[0].y == 17);
                CHECK(mp_out[1].x == 15);
                CHECK(mp_out[1].y == 12);
            }
        }
    }
    
    SECTION("line string")
    {
        mapbox::geometry::line_string<std::int64_t> ls_in;
        ls_in.emplace_back(25, 17);
        ls_in.emplace_back(15, 12);
        mapbox::geometry::geometry<std::int64_t> geom_in(ls_in);
        mapbox::feature::identifier id;
        mapbox::feature::feature<std::int64_t> feat { geom_in, prop_in, id };
        mapbox::vector_tile::encode_feature(lbuilder, feat);
        std::string buffer = tbuilder.serialize();
        auto const fc_map = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
        REQUIRE(fc_map.size() == 1);
        for (auto const& l : fc_map) 
        {
            REQUIRE(l.first == "my_layer_name");
            REQUIRE(l.second.size() == 1);
            for (auto const& f : l.second)
            {
                REQUIRE(f.id.is<mapbox::feature::null_value_t>());
                REQUIRE(f.geometry.is<mapbox::geometry::line_string<std::int64_t>>());
                prop_out = f.properties;
                auto ls_out = f.geometry.get<mapbox::geometry::line_string<std::int64_t>>();
                REQUIRE(ls_out.size() == 2);
                CHECK(ls_out[0].x == 25);
                CHECK(ls_out[0].y == 17);
                CHECK(ls_out[1].x == 15);
                CHECK(ls_out[1].y == 12);
            }
        }
    }

    // Check property values
    REQUIRE(prop_out.size() == 5);
    {
        REQUIRE(prop_out.end() != prop_out.find("a_string"));
        auto val = prop_out["a_string"];
        REQUIRE(val.is<std::string>());
        CHECK(val.get<std::string>() == "some_string");
    }
    {
        REQUIRE(prop_out.end() != prop_out.find("a_bool"));
        auto val = prop_out["a_bool"];
        REQUIRE(val.is<bool>());
        CHECK(val.get<bool>());
    }
    {
        REQUIRE(prop_out.end() != prop_out.find("a_int"));
        auto val = prop_out["a_int"];
        REQUIRE(val.is<std::int64_t>());
        CHECK(val.get<std::int64_t>() == -4);
    }
    {
        REQUIRE(prop_out.end() != prop_out.find("a_uint"));
        auto val = prop_out["a_uint"];
        REQUIRE(val.is<std::uint64_t>());
        CHECK(val.get<std::uint64_t>() == 12);
    }
    {
        REQUIRE(prop_out.end() != prop_out.find("a_double"));
        auto val = prop_out["a_double"];
        REQUIRE(val.is<double>());
        CHECK(val.get<double>() == 5.0);
    }
}
