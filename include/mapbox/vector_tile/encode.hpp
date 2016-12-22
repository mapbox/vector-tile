#pragma once

#include <mapbox/vector_tile/encode_layer.hpp>

#include <map>
#include <string>

namespace mapbox { namespace vector_tile {

template <typename CoordinateType>
std::string encode_tile(std::map<std::string, mapbox::geometry::feature_collection<CoordinateType>> layers,
                        std::uint32_t extent = 4096) {
    std::string buffer;
    for (auto const& l : layers) {
        encode_layer(buffer, l.first, l.second, extent);
    }
    return buffer;
}

}} // namespace mapbox::vector_tile
