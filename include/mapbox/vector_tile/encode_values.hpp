#pragma once

#include <mapbox/vector_tile/config.hpp>

#include <protozero/pbf_writer.hpp>

namespace mapbox { namespace vector_tile {

struct encode_value_visitor
{
	protozero::pbf_writer & value_;

    encode_value_visitor(protozero::pbf_writer & value):
        value_(value) {}
	
	template <typename T>
	void operator () (T const& /*val*/) const {
		// do nothing
	}

    void operator () (bool val) const {
        value_.add_bool(value_message::BOOL,val);
    }
    
	void operator () (std::uint64_t val) const {
        value_.add_uint64(value_message::UINT,val);
    }

    void operator () (std::int64_t val) const {
        value_.add_int64(value_message::INT,val);
    }

    void operator () (double val) const {
        float fval = static_cast<float>(val);
        if (val == static_cast<double>(fval))
        {
            value_.add_float(value_message::FLOAT, fval);
        }
        else
        {
            value_.add_double(value_message::DOUBLE, val);
        }
    }

    void operator () (std::string const& val) const {
        value_.add_string(value_message::STRING, val);
    }
};

void encode_value(protozero::pbf_writer & layer_writer, mapbox::geometry::value const& val) {
    protozero::pbf_writer value_writer(layer_writer, layer_message::VALUES);
    encode_value_visitor visitor(value_writer);
    mapbox::util::apply_visitor(visitor, val);
}

}} // namespace mapbox/vector_tile
