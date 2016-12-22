#pragma once

#include <mapbox/vector_tile/config.hpp>

#include <mapbox/geometry/geometry.hpp>
#include <protozero/pbf_writer.hpp>

namespace mapbox { namespace vector_tile {

template <typename Geometry>
std::size_t repeated_point_count(Geometry const& geom) {
    if (geom.size() < 2) {
        return 0;
    }
    std::size_t count = 0;
    auto itr = geom.begin();

    for (auto prev_itr = itr++; itr != geom.end(); ++prev_itr, ++itr) {
        if (itr->x == prev_itr->x && itr->y == prev_itr->y) {
            count++;
        }
    }
    return count;
}

inline std::uint32_t encode_length(std::uint32_t len) {
    return (len << 3u) | 2u;
}

template <typename CoordinateType>
bool encode_linestring(mapbox::geometry::line_string<CoordinateType> const& line,
                              protozero::packed_field_uint32 & geometry,
                              std::int32_t & start_x,
                              std::int32_t & start_y) {
    std::size_t line_size = line.size() - repeated_point_count(line);
    if (line_size < 2) {
        return false;
    }

    std::uint32_t line_to_length = static_cast<std::uint32_t>(line_size) - 1;

    auto pt = line.begin();
    geometry.add_element(9); // move_to | (1 << 3)
    geometry.add_element(protozero::encode_zigzag32(pt->x - start_x));
    geometry.add_element(protozero::encode_zigzag32(pt->y - start_y));
    start_x = pt->x;
    start_y = pt->y;
    geometry.add_element(encode_length(line_to_length));
    for (++pt; pt != line.end(); ++pt) {
        std::int32_t dx = pt->x - start_x;
        std::int32_t dy = pt->y - start_y;
        if (dx == 0 && dy == 0) {
            continue;
        }
        geometry.add_element(protozero::encode_zigzag32(dx));
        geometry.add_element(protozero::encode_zigzag32(dy));
        start_x = pt->x;
        start_y = pt->y;
    }
    return true;
}

template <typename CoordinateType>
bool encode_linearring(mapbox::geometry::linear_ring<CoordinateType> const& ring,
                              protozero::packed_field_uint32 & geometry,
                              std::int32_t & start_x,
                              std::int32_t & start_y) {
    std::size_t ring_size = ring.size() - repeated_point_count(ring);
    if (ring_size < 3) {
        return false;
    }
    auto last_itr = ring.end();
    if (ring.front() == ring.back()) {
        --last_itr;
        --ring_size;
        if (ring_size < 3) {
            return false;
        }
    }

    std::uint32_t line_to_length = static_cast<std::uint32_t>(ring_size) - 1;
    auto pt = ring.begin();
    geometry.add_element(9); // move_to | (1 << 3)
    geometry.add_element(protozero::encode_zigzag32(pt->x - start_x));
    geometry.add_element(protozero::encode_zigzag32(pt->y - start_y));
    start_x = pt->x;
    start_y = pt->y;
    geometry.add_element(encode_length(line_to_length));
    for (++pt; pt != last_itr; ++pt) {
        std::int32_t dx = pt->x - start_x;
        std::int32_t dy = pt->y - start_y;
        if (dx == 0 && dy == 0) {
            continue;
        }
        geometry.add_element(protozero::encode_zigzag32(dx));
        geometry.add_element(protozero::encode_zigzag32(dy));
        start_x = pt->x;
        start_y = pt->y;
    }
    geometry.add_element(15); // close_path
    return true;
}

template <typename CoordinateType>
bool encode_polygon(mapbox::geometry::polygon<CoordinateType> const& poly,
                           protozero::packed_field_uint32 & geometry,
                           std::int32_t & start_x,
                           std::int32_t & start_y) {
    if (poly.empty()) {
        return false;
    }
    bool first = true;
    for (auto const& ring : poly) {
        bool success = encode_linearring(ring, geometry, start_x, start_y);
        if (first) {
            if (!success) {
                return false;
            }
            first = false;
        }
    }
    return true;
}

template <typename CoordinateType>
void encode_point(mapbox::geometry::point<CoordinateType> const& pt,
                         protozero::packed_field_uint32 & geometry) {
    geometry.add_element(9);
    // Manual zigzag encoding.
    geometry.add_element(protozero::encode_zigzag32(pt.x));
    geometry.add_element(protozero::encode_zigzag32(pt.y));
}

template <typename CoordinateType>
bool encode_multi_point(mapbox::geometry::multi_point<CoordinateType> const& mp,
                               protozero::packed_field_uint32 & geometry) {
    std::size_t geom_size = mp.size();
    if (geom_size <= 0) {
        return false;
    }
    std::int32_t start_x = 0;
    std::int32_t start_y = 0;
    geometry.add_element(1u | (geom_size << 3)); // move_to | (len << 3)
    for (auto const& pt : mp) {
        std::int32_t dx = pt.x - start_x;
        std::int32_t dy = pt.y - start_y;
        // Manual zigzag encoding.
        geometry.add_element(protozero::encode_zigzag32(dx));
        geometry.add_element(protozero::encode_zigzag32(dy));
        start_x = pt.x;
        start_y = pt.y;
    }
    return true;
}

template <typename CoordinateType>
bool encode_geometry(mapbox::geometry::point<CoordinateType> const& pt,
                     protozero::pbf_writer & current_feature) {
    current_feature.add_enum(FeatureType::TYPE, GeomType::POINT);
    protozero::packed_field_uint32 geometry(current_feature, FeatureType::GEOMETRY);
    encode_point<CoordinateType>(pt, geometry);
    return true;
}

template <typename CoordinateType>
bool encode_geometry(mapbox::geometry::multi_point<CoordinateType> const& mp,
                     protozero::pbf_writer & current_feature) {
    current_feature.add_enum(FeatureType::TYPE, GeomType::POINT);
    protozero::packed_field_uint32 geometry(current_feature, FeatureType::GEOMETRY);
    return encode_multi_point<CoordinateType>(mp, geometry);
}

template <typename CoordinateType>
bool encode_geometry(mapbox::geometry::line_string<CoordinateType> const& line,
                     protozero::pbf_writer & current_feature) {
    current_feature.add_enum(FeatureType::TYPE, GeomType::LINESTRING);
    std::int32_t start_x = 0;
    std::int32_t start_y = 0;
    protozero::packed_field_uint32 geometry(current_feature, FeatureType::GEOMETRY);
    return encode_linestring(line, geometry, start_x, start_y);
}

template <typename CoordinateType>
bool encode_geometry(mapbox::geometry::multi_line_string<CoordinateType> const& geom,
                     protozero::pbf_writer & current_feature) {
    current_feature.add_enum(FeatureType::TYPE, GeomType::LINESTRING);
    bool success = false;
    std::int32_t start_x = 0;
    std::int32_t start_y = 0;
    protozero::packed_field_uint32 geometry(current_feature, FeatureType::GEOMETRY);
    for (auto const& line : geom) {
        if (encode_linestring(line, geometry, start_x, start_y)) {
            success = true;
        }
    }
    return success;
}

template <typename CoordinateType>
bool encode_geometry(mapbox::geometry::polygon<CoordinateType> const& poly,
                     protozero::pbf_writer & current_feature) {
    current_feature.add_enum(FeatureType::TYPE, GeomType::POLYGON);
    bool success = false;
    std::int32_t start_x = 0;
    std::int32_t start_y = 0;
    protozero::packed_field_uint32 geometry(current_feature, FeatureType::GEOMETRY);
    return encode_polygon(poly, geometry, start_x, start_y);
}

template <typename CoordinateType>
bool encode_geometry(mapbox::geometry::multi_polygon<CoordinateType> const& geom,
                     protozero::pbf_writer & current_feature) {
    current_feature.add_enum(FeatureType::TYPE, GeomType::POLYGON);
    bool success = false;
    std::int32_t start_x = 0;
    std::int32_t start_y = 0;
    protozero::packed_field_uint32 geometry(current_feature, FeatureType::GEOMETRY);
    for (auto const& poly : geom) {
        if (encode_polygon(poly, geometry, start_x, start_y)) {
            success = true;
        }
    }
    return success;
}

}} // namespace mapbox::vector_tile
