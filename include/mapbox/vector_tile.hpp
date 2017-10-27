#pragma once

#include <mapbox/geometry.hpp>
#include <mapbox/feature.hpp>
#include <vtzero/vector_tile.hpp>

#include <map>


namespace mapbox {
namespace vector_tile {

namespace detail {

template <typename CoordinateType>
struct point_geometry_handler
{

    using geom_type = mapbox::geometry::multi_point<CoordinateType>;

    geom_type& geom_;

    point_geometry_handler(geom_type& geom) : geom_(geom)
    {
    }

    void points_begin(std::uint32_t count)
    {
        geom_.reserve(count);
    }

    void points_point(const vtzero::point pt)
    {
        geom_.emplace_back(pt.x, pt.y);
    }

    void points_end()
    {
    }
};

template <typename CoordinateType>
mapbox::geometry::geometry<CoordinateType> create_geometry_point(vtzero::feature const& f)
{

    mapbox::geometry::multi_point<CoordinateType> mp;
    vtzero::decode_point_geometry(f.geometry(), false, detail::point_geometry_handler<CoordinateType>(mp));
    if (mp.empty())
    {
        return mapbox::geometry::geometry<CoordinateType>();
        // throw std::runtime_error("Point feature has no points in its geometry");
    }
    else if (mp.size() == 1)
    {
        return mapbox::geometry::geometry<CoordinateType>(mp.front());
    }
    else
    {
        return mapbox::geometry::geometry<CoordinateType>(std::move(mp));
    }
}

template <typename CoordinateType>
struct line_string_geometry_handler
{

    using geom_type = mapbox::geometry::multi_line_string<CoordinateType>;

    geom_type& geom_;

    line_string_geometry_handler(geom_type& geom) : geom_(geom)
    {
    }

    void linestring_begin(std::uint32_t count)
    {
        geom_.emplace_back();
        geom_.back().reserve(count);
    }

    void linestring_point(const vtzero::point pt)
    {
        geom_.back().emplace_back(pt.x, pt.y);
    }

    void linestring_end()
    {
    }
};

template <typename CoordinateType>
struct polygon_ring
{

    polygon_ring() : ring(), is_outer(true)
    {
    }

    mapbox::geometry::linear_ring<CoordinateType> ring;
    bool is_outer;
};

template <typename CoordinateType>
struct polygon_geometry_handler
{

    using geom_type = std::vector<polygon_ring<CoordinateType>>;

    geom_type& geom_;

    polygon_geometry_handler(geom_type& geom) : geom_(geom)
    {
    }

    void ring_begin(std::uint32_t count)
    {
        geom_.emplace_back();
        geom_.back().ring.reserve(count);
    }

    void ring_point(const vtzero::point pt)
    {
        geom_.back().ring.emplace_back(pt.x, pt.y);
    }

    void ring_end(bool is_outer)
    {
        geom_.back().is_outer = is_outer;
    }
};

template <typename CoordinateType>
mapbox::geometry::geometry<CoordinateType> create_geometry_polygon(vtzero::feature const& f)
{

    std::vector<polygon_ring<CoordinateType>> rings;
    vtzero::decode_polygon_geometry(f.geometry(), false, detail::polygon_geometry_handler<CoordinateType>(rings));
    if (rings.empty())
    {
        return mapbox::geometry::geometry<CoordinateType>();
        // throw std::runtime_error("Polygon feature has no rings in its geometry");
    }
    mapbox::geometry::multi_polygon<CoordinateType> mp;
    for (auto&& r : rings)
    {
        if (r.is_outer)
        {
            mp.emplace_back();
            mp.back().push_back(std::move(r.ring));
        }
        else if (!mp.empty())
        {
            mp.back().push_back(std::move(r.ring));
        }
    }
    if (mp.empty())
    {
        return mapbox::geometry::geometry<CoordinateType>();
        // throw std::runtime_error("Polygon feature has no rings in its geometry");
    }
    else if (mp.size() == 1)
    {
        return mapbox::geometry::geometry<CoordinateType>(std::move(mp.front()));
    }
    else
    {
        return mapbox::geometry::geometry<CoordinateType>(std::move(mp));
    }
}

template <typename CoordinateType>
mapbox::geometry::geometry<CoordinateType> create_geometry_line_string(vtzero::feature const& f)
{

    mapbox::geometry::multi_line_string<CoordinateType> mls;
    vtzero::decode_linestring_geometry(f.geometry(), false, detail::line_string_geometry_handler<CoordinateType>(mls));
    if (mls.empty())
    {
        return mapbox::geometry::geometry<CoordinateType>();
        // throw std::runtime_error("Line string feature has no points in its geometry");
    }
    else if (mls.size() == 1)
    {
        return mapbox::geometry::geometry<CoordinateType>(std::move(mls.front()));
    }
    else
    {
        return mapbox::geometry::geometry<CoordinateType>(std::move(mls));
    }
}

} // end ns detail

template <typename CoordinateType>
mapbox::geometry::geometry<CoordinateType> create_geometry(vtzero::feature const& f)
{
    switch (f.geometry_type())
    {
    case vtzero::GeomType::POINT:
        return detail::create_geometry_point<CoordinateType>(f);
    case vtzero::GeomType::LINESTRING:
        return detail::create_geometry_line_string<CoordinateType>(f);
    case vtzero::GeomType::POLYGON:
        return detail::create_geometry_polygon<CoordinateType>(f);
    default:
        return mapbox::geometry::geometry<CoordinateType>();
    }
}

struct property_value_mapping {

	/// mapping for string type
	using string_type = std::string;

	/// mapping for float type
	using float_type = double;

	/// mapping for double type
	using double_type = double;

	/// mapping for int type
	using int_type = int64_t;

	/// mapping for uint type
	using uint_type = uint64_t;

	/// mapping for bool type
	using bool_type = bool;

}; // struct property_value_mapping

mapbox::feature::property_map create_properties(vtzero::feature const& f)
{
    mapbox::feature::property_map map;
    f.for_each_property([&](vtzero::property && p) { 
		map.emplace(std::string(p.key()), vtzero::convert_property_value<mapbox::feature::value, property_value_mapping>(p.value()));
		return true;
	});
    return map;
}

mapbox::feature::identifier create_id(vtzero::feature const& f)
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
mapbox::feature::feature<CoordinateType> create_feature(vtzero::feature const& f)
{
    return mapbox::feature::feature<CoordinateType>(create_geometry<CoordinateType>(f), create_properties(f), create_id(f));
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
            auto f = create_feature<CoordinateType>(feature);
            if (!f.geometry.template is<mapbox::geometry::empty>())
            {
                fc.push_back(f);
            }
        }
        m.emplace(layer.name(), fc);
    }
    return m;
}
}
} // end ns mapbox::vector_tile
