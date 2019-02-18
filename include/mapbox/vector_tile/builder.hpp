#pragma once

#include <mapbox/geometry.hpp>
#include <mapbox/feature.hpp>

#include <vtzero/builder.hpp>

namespace mapbox {
namespace vector_tile {

template <typename T>
vtzero::point_2d convert(mapbox::geometry::point<T> pt) {
    return {static_cast<int32_t>(pt.x) , static_cast<int32_t>(pt.y)};
}

template <typename FeatureBuilder>
struct id_visitor
{

    FeatureBuilder& fbuilder;

    id_visitor(FeatureBuilder& fbuilder_) : fbuilder(fbuilder_) {}

    void operator()(std::uint64_t val)
    {
        fbuilder.set_integer_id(val);
    }

    void operator()(std::int64_t val)
    {
        fbuilder.set_integer_id(static_cast<std::uint64_t>(val));
    }

    template <typename T>
    void operator()(T /* val */)
    {
    }
};

template <typename FeatureBuilder>
struct value_visitor
{

    FeatureBuilder& fbuilder;
    std::string const& key;

    value_visitor(FeatureBuilder& fbuilder_, std::string const& key_) : fbuilder(fbuilder_), key(key_) {}

    void operator()(bool const& val)
    {
        fbuilder.add_property(key, vtzero::bool_value_type{val});
    }

    void operator()(std::uint64_t const& val)
    {
        fbuilder.add_property(key, vtzero::uint_value_type{val});
    }

    void operator()(std::int64_t const& val)
    {
        fbuilder.add_property(key, vtzero::int_value_type{val});
    }

    void operator()(double const& val)
    {
        fbuilder.add_property(key, vtzero::double_value_type{val});
    }

    void operator()(std::string const& val)
    {
        fbuilder.add_property(key, vtzero::string_value_type{val});
    }

    template <typename T>
    void operator()(T const& /* val */)
    {
    }
};

template <typename T>
std::uint32_t size_no_repeats(T const& geom)
{
    if (geom.empty())
    {
        return 0;
    }
    std::uint32_t size = geom.size() > std::numeric_limits<std::uint32_t>::max() ? std::numeric_limits<std::uint32_t>::max() : static_cast<std::uint32_t>(geom.size());
    for (std::size_t i = 1; i < geom.size(); ++i)
    {
        if (geom[i] == geom[i - 1])
        {
            --size;
        }
    }
    return size;
}

template <typename FeatureBuilder>
void set_properties(FeatureBuilder& fbuilder, mapbox::feature::property_map const& prop)
{
    for (auto const& n : prop)
    {
        mapbox::util::apply_visitor(value_visitor<FeatureBuilder>(fbuilder, n.first), n.second);
    }
}

struct feature_builder_visitor
{

    vtzero::layer_builder& lbuilder;
    mapbox::feature::property_map const& prop;
    mapbox::feature::identifier const& id;

    feature_builder_visitor(vtzero::layer_builder& lbuilder_,
                            mapbox::feature::property_map const& prop_,
                            mapbox::feature::identifier const& id_) : lbuilder(lbuilder_),
                                                                      prop(prop_),
                                                                      id(id_) {}

    template <typename T>
    feature_builder_visitor(vtzero::layer_builder& lbuilder_,
                            mapbox::feature::feature<T> const& feat) : lbuilder(lbuilder_),
                                                                       prop(feat.properties),
                                                                       id(feat.id)
    {
    }

    void operator()(mapbox::geometry::empty const& /* geom */) {}

    template <typename T>
    void operator()(mapbox::geometry::point<T> const& pt)
    {
        using builder_type = vtzero::point_feature_builder<2>;
        builder_type fbuilder{lbuilder};
        mapbox::util::apply_visitor(id_visitor<builder_type>(fbuilder), id);
        fbuilder.add_point(convert(pt));
        set_properties(fbuilder, prop);
        fbuilder.commit();
    }

    template <typename T>
    void operator()(mapbox::geometry::multi_point<T> const& mp)
    {
        using builder_type = vtzero::point_feature_builder<2>;
        if (mp.empty())
        {
            return;
        }
        builder_type fbuilder{lbuilder};
        mapbox::util::apply_visitor(id_visitor<builder_type>(fbuilder), id);
        fbuilder.add_points(static_cast<uint32_t>(mp.size()));
        for (auto const& pt : mp) {
            fbuilder.set_point(convert(pt));
        }
        set_properties(fbuilder, prop);
        fbuilder.commit();
    }

    template <typename T>
    void operator()(mapbox::geometry::line_string<T> const& ls)
    {
        using builder_type = vtzero::linestring_feature_builder<2>;
        std::uint32_t s = size_no_repeats(ls);
        if (s < 2)
        {
            return;
        }
        builder_type fbuilder{lbuilder};
        mapbox::util::apply_visitor(id_visitor<builder_type>(fbuilder), id);
        fbuilder.add_linestring(s);
        fbuilder.set_point(convert(ls.front()));
        for (std::size_t i = 1; i < ls.size(); ++i)
        {
            if (ls[i] != ls[i - 1])
            {
                fbuilder.set_point(convert(ls[i]));
            }
        }
        set_properties(fbuilder, prop);
        fbuilder.commit();
    }

    template <typename T>
    void operator()(mapbox::geometry::multi_line_string<T> const& mls)
    {
        using builder_type = vtzero::linestring_feature_builder<2>;
        if (mls.empty())
        {
            return;
        }
        bool empty = true;
        builder_type fbuilder{lbuilder};
        mapbox::util::apply_visitor(id_visitor<builder_type>(fbuilder), id);
        for (auto const& ls : mls)
        {
            std::uint32_t s = size_no_repeats(ls);
            if (s < 2)
            {
                continue;
            }
            fbuilder.add_linestring(s);
            fbuilder.set_point(convert(ls.front()));
            for (std::size_t i = 1; i < ls.size(); ++i)
            {
                if (ls[i] != ls[i - 1])
                {
                    fbuilder.set_point(convert(ls[i]));
                }
            }
            empty = false;
        }
        if (empty)
        {
            fbuilder.rollback();
        }
        else
        {
            set_properties(fbuilder, prop);
            fbuilder.commit();
        }
    }

    template <typename T>
    void operator()(mapbox::geometry::polygon<T> const& poly)
    {
        using builder_type = vtzero::polygon_feature_builder<2>;
        if (poly.empty())
        {
            return;
        }
        builder_type fbuilder{lbuilder};
        mapbox::util::apply_visitor(id_visitor<builder_type>(fbuilder), id);
        bool first = true;
        for (auto const& ring : poly)
        {
            std::uint32_t s = size_no_repeats(ring);
            if (first && s < 4)
            {
                fbuilder.rollback();
                return;
            }
            else if (s >= 4)
            {
                fbuilder.add_ring(s);
                fbuilder.set_point(convert(ring.front()));
                for (std::size_t i = 1; i < ring.size(); ++i)
                {
                    if (ring[i] != ring[i - 1])
                    {
                        fbuilder.set_point(convert(ring[i]));
                    }
                }
            }
            first = false;
        }
        set_properties(fbuilder, prop);
        fbuilder.commit();
    }

    template <typename T>
    void operator()(mapbox::geometry::multi_polygon<T> const& mp)
    {
        using builder_type = vtzero::polygon_feature_builder<2>;
        builder_type fbuilder{lbuilder};
        mapbox::util::apply_visitor(id_visitor<builder_type>(fbuilder), id);
        for (auto const& poly : mp)
        {
            bool first = true;
            for (auto const& ring : poly)
            {
                std::uint32_t s = size_no_repeats(ring);
                if (first && s < 4)
                {
                    fbuilder.rollback();
                    return;
                }
                else if (s >= 4)
                {
                    fbuilder.add_ring(s);
                    fbuilder.set_point(convert(ring.front()));
                    for (std::size_t i = 1; i < ring.size(); ++i)
                    {
                        if (ring[i] != ring[i - 1])
                        {
                            fbuilder.set_point(convert(ring[i]));
                        }
                    }
                }
                first = false;
            }
        }
        set_properties(fbuilder, prop);
        fbuilder.commit();
    }

    template <typename T>
    void operator()(mapbox::geometry::geometry_collection<T> const& gc)
    {
        for (auto const& geom : gc)
        {
            mapbox::util::apply_visitor(*this, geom);
        }
    }
};

template <typename T>
void encode_feature(vtzero::layer_builder& lbuilder, mapbox::feature::feature<T> const& feat)
{
    mapbox::util::apply_visitor(feature_builder_visitor(lbuilder, feat), feat.geometry);
}
}
}
