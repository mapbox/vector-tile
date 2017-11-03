#pragma once

#include <mapbox/geometry.hpp>
#include <vtzero/builder.hpp>

namespace vtzero {

    template <>
    point create_point<mapbox::geometry::point<std::int64_t>>(mapbox::geometry::point<std::int64_t> p) noexcept {
        return {static_cast<std::int32_t>(p.x),
                static_cast<std::int32_t>(p.y)};
    }
    
    template <>
    point create_point<mapbox::geometry::point<std::int32_t>>(mapbox::geometry::point<std::int32_t> p) noexcept {
        return { p.x, p.y };
    }
    
    template <>
    point create_point<mapbox::geometry::point<std::int16_t>>(mapbox::geometry::point<std::int16_t> p) noexcept {
        return {static_cast<std::int32_t>(p.x),
                static_cast<std::int32_t>(p.y)};
    }

} // namespace vtzero

namespace mapbox {
namespace vector_tile {

template <typename FeatureBuilder>
struct id_visitor {

    FeatureBuilder & fbuilder;

    id_visitor(FeatureBuilder & fbuilder_) : fbuilder(fbuilder_) {}
    
    void operator() (std::uint64_t val)
    {
        fbuilder.set_id(val);
    }
    
    void operator() (std::int64_t val)
    {
        std::uint64_t val_ = static_cast<std::uint64_t>(val);
        if (static_cast<std::int64_t>(val_) == val)
        {
            fbuilder.set_id(val_);
        }
    }

    template <typename T>
    void operator() (T /* val */) {}

};

template <typename FeatureBuilder>
struct value_visitor {

    FeatureBuilder & fbuilder;
    std::string const& key;

    value_visitor(FeatureBuilder & fbuilder_, std::string const& key_) : fbuilder(fbuilder_), key(key_) {}

    void operator() (bool const& val)
    {
        fbuilder.add_property(key, vtzero::bool_value_type{val});
    }
    
    void operator() (std::uint64_t const& val)
    {
        fbuilder.add_property(key, vtzero::uint_value_type{val});
    }
    
    void operator() (std::int64_t const& val)
    {
        fbuilder.add_property(key, vtzero::int_value_type{val});
    }
    
    void operator() (double const& val)
    {
        fbuilder.add_property(key, vtzero::double_value_type{val});
    }
    
    void operator() (std::string const& val)
    {
        fbuilder.add_property(key, vtzero::string_value_type{val});
    }

    template <typename T>
    void operator() (T const & /* val */) 
    {
    }

};

template <typename FeatureBuilder>
void set_properties(FeatureBuilder & fbuilder, mapbox::feature::property_map const & prop)
{
    for (auto const& n : prop)
    {
        mapbox::apply_visitor(value_visitor(fbuilder, n.first), n.second);  
    }
}

struct feature_builder_visitor {
    
    vtzero::layer_builder & lbuilder;
    mapbox::feature::property_map const& prop;
    mapbox::feature::identifier const& id;

    feature_builder_visitor(vtzero::layer_builder & lbuilder_,
                            mapbox::feature::property_map const& prop_, 
                            mapbox::feature::identifier const& id_) :
        lbuilder(lbuilder_),
        prop(prop_),
        id(id_) {}
    
    feature_builder_visitor(vtzero::layer_builder & lbuilder_,
                            mapbox::feature::feature & feat) : 
        lbuilder(lbuilder_),
        prop(feat.properties),
        id(feat.id) {}

    void operator() (mapbox::geometry::empty const& /* geom */) {}

    template <typename T>
    void operator() (mapbox::geometry::point<T> const& pt)
    {
        vtzero::point_feature_builder fbuilder { lbuilder };
        mapbox::apply_visitor(id_visitor(fbuilder), id);
        fbuilder.add_point(pt);
        set_properties(fbuilder, prop);
        fbuilder.commit();
    }

    template <typename T>
    void operator() (mapbox::geometry::multi_point<T> const& mp)
    {
        vtzero::point_feature_builder fbuilder { lbuilder };
        mapbox::apply_visitor(id_visitor(fbuilder), id);
        fbuilder.add_points_from_container(mp);
        set_properties(fbuilder, prop);
        fbuilder.commit();
    }
    
    template <typename T>
    void operator() (mapbox::geometry::line_string<T> const& ls)
    {
        vtzero::linestring_feature_builder fbuilder { lbuilder };
        mapbox::apply_visitor(id_visitor(fbuilder), id);
        fbuilder.add_linestring_from_container(ls);
        set_properties(fbuilder, prop);
        fbuilder.commit();
    }

    template <typename T>
    void operator() (mapbox::geometry::multi_line_string<T> const& mls)
    {
        vtzero::linestring_feature_builder fbuilder { lbuilder };
        mapbox::apply_visitor(id_visitor(fbuilder), id);
        for (auto const& ls : mls)
        {
            fbuilder.add_linestring_from_container(ls);
        }
        set_properties(fbuilder, prop);
        fbuilder.commit();
    }
    
    template <typename T>
    void operator() (mapbox::geometry::polygon<T> const& poly)
    {
        vtzero::polygon_feature_builder fbuilder { lbuilder };
        mapbox::apply_visitor(id_visitor(fbuilder), id);
        for (auto const & ring : poly)
        {
            fbuilder.add_ring_from_container(ring);
        }
        set_properties(fbuilder, prop);
        fbuilder.commit();
    }

    template <typename T>
    void operator() (mapbox::geometry::multi_polygon<T> const& mp)
    {
        vtzero::linestring_feature_builder fbuilder { lbuilder };
        mapbox::apply_visitor(id_visitor(fbuilder), id);
        for (auto const & poly : mp)
        {
            for (auto const & ring : poly)
            {
                fbuilder.add_ring_from_container(ring);
            }
        }
        set_properties(fbuilder, prop);
        fbuilder.commit();
    }
    
    template <typename T>
    void operator() (mapbox::geometry::geometry_collection<T> const& gc)
    {
        for (auto const& geom : gc)
        {
            mapbox::apply_visitor(*this, geom);
        }
    }

};



}
}
