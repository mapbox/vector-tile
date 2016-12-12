#pragma once

#include <mapbox/vector_tile/config.hpp>
#include <mapbox/vector_tile/encode_values.hpp>
#include <mapbox/vector_tile/encode_geometry.hpp>

#include <mapbox/geometry/feature.hpp>

#include <protozero/pbf_writer.hpp>

#include <deque>
#include <map>
#include <unordered_map>
#include <string>

#include <experimental/optional>

// This enables mapbox::geometry::value to be hashable
namespace mapbox { namespace vector_tile {
struct hash_visitor {
    template <typename T>
    std::size_t operator()(T const& h) const {
        return std::hash<T>{}(h);
    }
};

}} // namespace mapbox::vector_tile

namespace std {
template <>
struct hash<::mapbox::geometry::null_value_t> {
    std::size_t operator()(::mapbox::geometry::null_value_t const& ) const noexcept {
        return 0;
    }
};

template <>
struct hash<vector<::mapbox::geometry::value>> {
    std::size_t operator()(vector<::mapbox::geometry::value> const& ) const noexcept {
        return 1;
    }
};

template <>
struct hash<unordered_map<string, ::mapbox::geometry::value>> {
    std::size_t operator()(unordered_map<string, ::mapbox::geometry::value> const& ) const noexcept {
        return 2;
    }
};

template <>
struct hash<::mapbox::geometry::value> {
    std::size_t operator()(::mapbox::geometry::value const& v) const noexcept {
        return ::mapbox::util::apply_visitor(::mapbox::vector_tile::hash_visitor{}, v);
    }
};
} // namespace std

namespace mapbox { namespace vector_tile {

typedef std::map<std::string, std::uint32_t> layer_keys_container;
typedef std::unordered_map<mapbox::geometry::value, std::uint32_t> layer_values_container;

inline std::deque<std::uint32_t> encode_properties_to_layer(protozero::pbf_writer & layer_writer,
															layer_keys_container & layer_keys,
															layer_values_container & layer_values,
															mapbox::geometry::property_map const& properties) {
    std::deque<std::uint32_t> feature_tags;
    for (auto const& p : properties) {
        if (p.second.is<mapbox::geometry::null_value_t>()) {
            continue;
        }
		layer_keys_container::const_iterator key_itr = layer_keys.find(p.first);
		if (key_itr == layer_keys.end()) {   
			// The key doesn't exist yet in the dictionary.
			layer_writer.add_string(layer_message::KEYS, p.first);
			std::size_t index = layer_keys.size();
			layer_keys.emplace(p.first, index);
			feature_tags.push_back(index);
		} else {
			feature_tags.push_back(key_itr->second);
		}
		
		layer_values_container::const_iterator val_itr = layer_values.find(p.second);
		if (val_itr == layer_values.end()) {
			encode_value(layer_writer, p.second); 
			std::size_t index = layer_values.size();
			layer_values.emplace(p.second, index);
			feature_tags.push_back(index);
		} else {
			feature_tags.push_back(val_itr->second);
		}
    }
    return feature_tags;
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

void encode_id(protozero::pbf_writer & feature_writer, mapbox::geometry::identifier const& id) {
    encode_id_visitor visitor(feature_writer);
    mapbox::util::apply_visitor(visitor, id);
}

template <typename CoordinateType, typename GeometryType>
void encode_feature_geometry(protozero::pbf_writer & layer_writer,
						     std::deque<std::uint32_t> const& feature_tags,
                             std::experimental::optional<mapbox::geometry::identifier> const& id,
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

    std::deque<std::uint32_t> const& feature_tags;
	std::experimental::optional<mapbox::geometry::identifier> const& id;
    protozero::pbf_writer & layer_writer;

	encode_feature_visitor(std::deque<std::uint32_t> const& f,
                           std::experimental::optional<mapbox::geometry::identifier> const& i,
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
            mapbox::util::apply_visitor((*this),geom);
        }
    }

};

template <typename CoordinateType>
void encode_feature(protozero::pbf_writer & layer_writer, 
                    layer_keys_container & layer_keys,
                    layer_values_container & layer_values,
                    mapbox::geometry::feature<CoordinateType> const& feature) {
    std::deque<std::uint32_t> feature_tags = encode_properties_to_layer(layer_writer, layer_keys, layer_values, feature.properties);
    encode_feature_visitor<CoordinateType> visitor(feature_tags, feature.id, layer_writer);
    mapbox::util::apply_visitor(visitor, feature.geometry);
}

}} // namespace mapbox::vector_tile
