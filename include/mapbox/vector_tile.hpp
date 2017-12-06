#pragma once

#include <mapbox/geometry.hpp>
#include <mapbox/feature.hpp>
#include <mapbox/vector_tile/handlers.hpp>
#include <mapbox/vector_tile/property_map.hpp>
#include <vtzero/vector_tile.hpp>

#include <map>

namespace mapbox {
namespace vector_tile {

template <typename CoordinateType>
mapbox::geometry::geometry<CoordinateType> extract_geometry(vtzero::feature const& f)
{
    switch (f.geometry_type())
    {
    case vtzero::GeomType::POINT:
        return detail::extract_geometry_point<CoordinateType>(f);
    case vtzero::GeomType::LINESTRING:
        return detail::extract_geometry_line_string<CoordinateType>(f);
    case vtzero::GeomType::POLYGON:
        return detail::extract_geometry_polygon<CoordinateType>(f);
    default:
        return mapbox::geometry::geometry<CoordinateType>();
    }
}

inline mapbox::feature::property_map extract_properties(vtzero::feature const& f)
{
    mapbox::feature::property_map map;
    f.for_each_property([&](vtzero::property&& p) {
        map.emplace(std::string(p.key()), vtzero::convert_property_value<mapbox::feature::value, detail::property_value_mapping>(p.value()));
        return true;
    });
    return map;
}

inline mapbox::feature::identifier extract_id(vtzero::feature const& f)
{
    if (f.has_id())
    {
        return mapbox::feature::identifier(f.id());
    }
    else
    {
        return mapbox::feature::identifier();
    }
}

template <typename CoordinateType>
mapbox::feature::feature<CoordinateType> extract_feature(vtzero::feature const& f)
{
    return mapbox::feature::feature<CoordinateType>(extract_geometry<CoordinateType>(f), extract_properties(f), extract_id(f));
}

template <typename CoordinateType>
using layer_map = std::map<std::string, mapbox::feature::feature_collection<CoordinateType>>;

template <typename CoordinateType>
layer_map<CoordinateType> decode_tile(std::string const& buffer)
{
    layer_map<CoordinateType> m;
    vtzero::vector_tile tile(buffer);
    while (auto layer = tile.next_layer())
    {
        mapbox::feature::feature_collection<CoordinateType> fc;
        while (auto feature = layer.next_feature())
        {
            auto f = extract_feature<CoordinateType>(feature);
            if (!f.geometry.template is<mapbox::geometry::empty>())
            {
                fc.push_back(f);
            }
        }

        if (!fc.empty())
        {
            m.emplace(std::string(layer.name()), std::move(fc));
        }
    }
    return m;
}
}
} // end ns mapbox::vector_tile
