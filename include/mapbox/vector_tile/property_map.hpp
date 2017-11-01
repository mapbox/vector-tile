#pragma once

namespace mapbox {
namespace vector_tile {
namespace detail {

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

}
}
}
