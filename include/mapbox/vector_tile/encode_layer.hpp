#pragma once

#include <mapbox/vector_tile/config.hpp>
#include <mapbox/vector_tile/encode_feature.hpp>

#include <protozero/pbf_writer.hpp>

#include <deque>

namespace mapbox { namespace vector_tile {

void encode_layer_general_data(protozero::pbf_writer & layer_writer, std::string const& name, std::uint32_t extent, std::uint32_t version) {
    layer_writer.add_uint32(layer_message::VERSION, version);
    layer_writer.add_string(layer_message::NAME, name);
    layer_writer.add_uint32(layer_message::EXTENT, extent);
}

template <typename CoordinateType>
void encode_layer(std::string & buffer, 
                  std::string const& name,
                  mapbox::geometry::feature_collection<CoordinateType> const& features, 
                  std::uint32_t extent = 4096,
                  std::uint32_t version = 2) {

    protozero::pbf_writer tile_writer(buffer);
    protozero::pbf_writer layer_writer(tile_writer, tile_message::LAYER);
    encode_layer_general_data(layer_writer, name, extent, version);
	layer_keys_container layer_keys;
	layer_values_container layer_values;

    for (auto const& f : features) {
		encode_feature<CoordinateType>(layer_writer, layer_keys, layer_values, f);
    }
}

}} // namespace mapbox::vector_tile
