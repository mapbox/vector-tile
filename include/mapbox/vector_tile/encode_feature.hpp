#pragma once

#include <mapbox/vector_tile/config.hpp>
#include <mapbox/vector_tile/encode_values.hpp>
#include <mapbox/vector_tile/encode_geometry.hpp>

#include <mapbox/geometry/feature.hpp>

#include <protozero/pbf_writer.hpp>

#include <deque>
#include <map>
#include <unordered_map>
#include <experimental/optional>

namespace mapbox { namespace vector_tile {

typedef std::map<std::string, std::uint32_t> layer_keys_container;
typedef std::unordered_map<mapbox::geometry::value, std::uint32_t> layer_values_container;

inline std::deque<std::uint32_t> encode_properties_to_layer(protozero::pbf_writer & layer_writer,
															layer_keys_container & layer_keys,
															layer_values_container & layer_values,
															mapbox::geometry::property_map const& properties,
															std::deque<std::uint32_t> & feature_tags) {
    std::deque<std::uint32_t> feature_tags;
    
    for (auto const& p : properties) {
        if (p.second.is<mapbox::geometry::null_value_t>()) {
            continue;
        }
		keys_container::const_iterator key_itr = layer_keys.find(p.first);
		if (key_itr == keys.end()) {   
			// The key doesn't exist yet in the dictionary.
			layer_writer.add_string(layer_message::KEYS, p.first);
			std::size_t index = keys.size();
			keys.emplace(p.first, index);
			feature_tags.push_back(index);
		} else {
			feature_tags.push_back(key_itr->second);
		}
		
		values_container::const_iterator val_itr = layer_values.find(p.second);
		if (val_itr == values.end()) {
			encode_value(layer_writer, p.second); 
			std::size_t index = values.size();
			values.emplace(p.second, index);
			feature_tags.push_back(index);
		} else {
			feature_tags.push_back(val_itr->second);
		}
    }
}

struct encode_id_visitor
{
	protozero::pbf_writer & id_;

    encode_id_visitor(protozero::pbf_writer & id):
        id_(id) {}
	
    void operator () (std::string const&) const {
		// do nothing
    }

	template <typename T>
	void operator () (T const& val) const {
        id_.add_uint64(feature_message::ID, static_cast<std::uint64_t>(val));
	}
};

void encode_id(protozero::pbf_writer & feature_writer, mapbox::geometry::identifer const& id) {
    encode_id_visitor visitor(feature_writer);
    mapbox::apply_visitor(visitor, id);
}

template <typename CoordinateType, typename GeometryType>
void encode_feature_geometry(protozero::pbf_writer & layer_writer,
						     std::deque<std::uint32_t> const& feature_tags,
                             std::experimental::optional<identifier> const& id,
                             GeometryType const& geometry) {
	protozero::pbf_writer feature_writer(layer_writer, layer_message::FEATURES);
	bool success = encode_geometry<CoordinateType>(geometry, feature_writer);
	if (success) {
		if (id) {
			encode_id(feature_writer, *id);
		}
		feature_writer.add_packed_uint32(feature_message::TAGS, feature_tags.begin(), feature_tags.end());
	} else {
		feature_writer.rollback();
	}
}

template <typename CoordinateType>
struct encode_feature_visitor {

    std::deque<std::uint32_t> const& feature_tags,
	std::experimental::optional<identifier> const& id;
    protozero::pbf_writer & layer_writer;

	encode_feature_visitor(std::deque<std::uint32_t> const& f,
                           std::experimental::optional<identifier> const& i,
                           protozero::pbf_writer & l)
        : feature_tags(f),
          id(i),
          layer_writer(l) {}

	template <typename GeometryType>
	void operator() (GeometryType const& geom) {
		encode_feature_geometry<CoordinateType, GeometryType>(layer_writer, feature_tags, id, geom);
	}

    void operator() (mapbox::geometry::geometry_collection<CoordinateType> const& geom_collection) {
        for (auto const& geom : geom_collection) {
            (*this)(geom);
        }
    }

};

template <typename CoordinateType>
void encode_feature(std::string & buffer, 
                    layer_keys_container & layer_keys,
                    layer_values_container & layer_values,
                    mapbox::geometry::feature<CoordinateType> const& feature) {

	std::deque<std::uint32_t> feature_tags;
    protozero::pbf_writer layer_writer(buffer);
    encode_properties_to_layer(layer_writer, layer_keys, layer_values, feature.properties, feature_tags);
    encode_feature_visitor<CoordinateType> visitor(feature_tags, feature.id, layer_writer);
    mapbox::apply_visitor(visitor, feature.geometry);
}

}} // namespace mapbox::vector_tile
