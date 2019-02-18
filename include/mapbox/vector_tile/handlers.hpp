#pragma once

#include <mapbox/geometry.hpp>
#include <vtzero/vector_tile.hpp>

namespace mapbox {
namespace vector_tile {
namespace detail {

template <typename CoordinateType>
struct point_geometry_handler
{
    constexpr static const int dimensions = 2;
    constexpr static const unsigned int max_geometric_attributes = 0;

    using geom_type = mapbox::geometry::multi_point<CoordinateType>;

    geom_type& geom_;

    point_geometry_handler(geom_type& geom) : geom_(geom)
    {
    }

    void points_begin(std::uint32_t count)
    {
        geom_.reserve(count);
    }

    void points_point(const vtzero::point_2d pt)
    {
        geom_.emplace_back(pt.x, pt.y);
    }

    void points_end()
    {
    }

};

template <typename CoordinateType>
mapbox::geometry::geometry<CoordinateType> extract_geometry_point(vtzero::feature const& f)
{

    mapbox::geometry::multi_point<CoordinateType> mp;
    f.decode_point_geometry(detail::point_geometry_handler<CoordinateType>(mp));
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
    constexpr static const int dimensions = 2;
    constexpr static const unsigned int max_geometric_attributes = 0;

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

    void linestring_point(const vtzero::point_2d pt)
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

    polygon_ring() : ring(), type(vtzero::ring_type::invalid)
    {
    }

    mapbox::geometry::linear_ring<CoordinateType> ring;
    vtzero::ring_type type;
};

template <typename CoordinateType>
struct polygon_geometry_handler
{
    constexpr static const int dimensions = 2;
    constexpr static const unsigned int max_geometric_attributes = 0;

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

    void ring_point(const vtzero::point_2d pt)
    {
        geom_.back().ring.emplace_back(pt.x, pt.y);
    }

    void ring_end(vtzero::ring_type type)
    {
        geom_.back().type = type;
    }
};

template <typename CoordinateType>
mapbox::geometry::geometry<CoordinateType> extract_geometry_polygon(vtzero::feature const& f)
{

    std::vector<polygon_ring<CoordinateType>> rings;
    f.decode_polygon_geometry(detail::polygon_geometry_handler<CoordinateType>(rings));
    if (rings.empty())
    {
        return mapbox::geometry::geometry<CoordinateType>();
        // throw std::runtime_error("Polygon feature has no rings in its geometry");
    }
    mapbox::geometry::multi_polygon<CoordinateType> mp;
    mp.reserve(rings.size());
    for (auto&& r : rings)
    {
        if (r.type == vtzero::ring_type::outer)
        {
            mp.emplace_back();
            mp.back().push_back(std::move(r.ring));
        }
        else if (!mp.empty() && r.type == vtzero::ring_type::inner)
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
mapbox::geometry::geometry<CoordinateType> extract_geometry_line_string(vtzero::feature const& f)
{

    mapbox::geometry::multi_line_string<CoordinateType> mls;
    f.decode_linestring_geometry(detail::line_string_geometry_handler<CoordinateType>(mls));
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
}
}
