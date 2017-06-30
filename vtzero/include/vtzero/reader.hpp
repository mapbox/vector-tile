#ifndef VTZERO_READER_HPP
#define VTZERO_READER_HPP

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

#include <protozero/pbf_message.hpp>

#include "exception.hpp"
#include "geometry.hpp"
#include "types.hpp"

namespace vtzero {

    /**
     * A vector tile tag.
     */
    class tag {

        data_view m_key;
        data_view m_value;

        protozero::pbf_message<detail::pbf_value> check_value(detail::pbf_value type, protozero::pbf_wire_type wire_type) const {
            protozero::pbf_message<detail::pbf_value> value_message{m_value};

            if (!value_message.next(type, wire_type)) {
                throw type_exception{};
            }

            return value_message;
        }

    public:

        tag() = default;

        tag(const data_view& key, const data_view& value) noexcept :
            m_key(key),
            m_value(value) {
        }

        bool valid() const noexcept {
            return m_key.data() != nullptr;
        }

        operator bool() const noexcept {
            return valid();
        }

        const data_view& key() const noexcept {
            return m_key;
        }

        const data_view& value() const noexcept {
            return m_value;
        }

        value_type get_value_type() const {
            protozero::pbf_message<detail::pbf_value> value_message{m_value};
            if (value_message.next()) {
                return value_message.tag();
            }
            throw format_exception{};
        }

        data_view string_value() const {
            return check_value(detail::pbf_value::string_value, protozero::pbf_wire_type::length_delimited).get_view();
        }

        float float_value() const {
            return check_value(detail::pbf_value::float_value, protozero::pbf_wire_type::fixed32).get_float();
        }

        double double_value() const {
            return check_value(detail::pbf_value::double_value, protozero::pbf_wire_type::fixed64).get_double();
        }

        std::int64_t int_value() const {
            return check_value(detail::pbf_value::int_value, protozero::pbf_wire_type::varint).get_int64();
        }

        std::uint64_t uint_value() const {
            return check_value(detail::pbf_value::uint_value, protozero::pbf_wire_type::varint).get_uint64();
        }

        std::int64_t sint_value() const {
            return check_value(detail::pbf_value::sint_value, protozero::pbf_wire_type::varint).get_sint64();
        }

        bool bool_value() const {
            return check_value(detail::pbf_value::bool_value, protozero::pbf_wire_type::varint).get_bool();
        }

    }; // class tag

    template <typename V>
    void tag_value_visit(V&& visitor, const tag& tag) {
        switch (tag.get_value_type()) {
            case detail::pbf_value::string_value:
                std::forward<V>(visitor)(tag.string_value());
                break;
            case detail::pbf_value::float_value:
                std::forward<V>(visitor)(tag.float_value());
                break;
            case detail::pbf_value::double_value:
                std::forward<V>(visitor)(tag.double_value());
                break;
            case detail::pbf_value::int_value:
                std::forward<V>(visitor)(tag.int_value());
                break;
            case detail::pbf_value::uint_value:
                std::forward<V>(visitor)(tag.uint_value());
                break;
            case detail::pbf_value::sint_value:
                std::forward<V>(visitor)(tag.sint_value());
                break;
            case detail::pbf_value::bool_value:
                std::forward<V>(visitor)(tag.bool_value());
                break;
        }
    }

    class layer;

    class tags_iterator {

        protozero::pbf_reader::const_uint32_iterator m_it;
        protozero::pbf_reader::const_uint32_iterator m_end;
        const layer* m_layer;

    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type        = tag;
        using difference_type   = std::ptrdiff_t;
        using pointer           = value_type*;
        using reference         = value_type&;

        tags_iterator(const protozero::pbf_reader::const_uint32_iterator& begin,
                      const protozero::pbf_reader::const_uint32_iterator& end,
                      const layer* layer) :
            m_it(begin),
            m_end(end),
            m_layer(layer) {
            assert(layer);
        }

        tag operator*() const;

        tags_iterator& operator++() {
            ++m_it;
            if (m_it == m_end) {
                throw format_exception{};
            }
            ++m_it;
            return *this;
        }

        tags_iterator operator++(int) {
            const tags_iterator tmp{*this};
            ++(*this);
            return tmp;
        }

        bool operator==(const tags_iterator& other) const noexcept {
            return m_it == other.m_it &&
                   m_end == other.m_end &&
                   m_layer == other.m_layer;
        }

        bool operator!=(const tags_iterator& other) const noexcept {
            return !(*this == other);
        }

    }; // tags_iterator

    /**
     * A feature according to spec 4.2.
     */
    class feature {

        using uint32_it_range = protozero::iterator_range<protozero::pbf_reader::const_uint32_iterator>;

        uint64_t m_id;
        uint32_it_range m_tags;
        data_view m_geometry;
        GeomType m_type;

    public:

        feature() :
            m_id(0),
            m_tags(),
            m_geometry(),
            m_type() {
        }

        feature(const data_view& data) :
            m_id(0), // defaults to 0, see https://github.com/mapbox/vector-tile-spec/blob/master/2.1/vector_tile.proto#L32
            m_tags(),
            m_geometry(),
            m_type() { // defaults to UNKOWN, see https://github.com/mapbox/vector-tile-spec/blob/master/2.1/vector_tile.proto#L41

            protozero::pbf_message<detail::pbf_feature> reader{data};

            while (reader.next()) {
                switch (reader.tag_and_type()) {
                    case protozero::tag_and_type(detail::pbf_feature::id, protozero::pbf_wire_type::varint):
                        m_id = reader.get_uint64();
                        break;
                    case protozero::tag_and_type(detail::pbf_feature::tags, protozero::pbf_wire_type::length_delimited):
                        m_tags = reader.get_packed_uint32();
                        break;
                    case protozero::tag_and_type(detail::pbf_feature::type, protozero::pbf_wire_type::varint): {
                            const auto type = reader.get_enum();
                            // spec 4.3.4 "Geometry Types"
                            if (type < 0 || type > 3) {
                                throw format_exception{};
                            }
                            m_type = static_cast<GeomType>(type);
                        }
                        break;
                    case protozero::tag_and_type(detail::pbf_feature::geometry, protozero::pbf_wire_type::length_delimited):
                        m_geometry = reader.get_view();
                        break;
                    default:
                        reader.skip();
                }
            }

            // spec 4.2 "A feature MUST contain a geometry field."
            if (m_geometry.empty()) {
                throw format_exception{};
            }
        }

        bool valid() const noexcept {
            return m_geometry.data() != nullptr;
        }

        operator bool() const noexcept {
            return valid();
        }

        uint64_t id() const noexcept {
            assert(valid());
            return m_id;
        }

        GeomType type() const noexcept {
            assert(valid());
            return m_type;
        }

        const data_view& geometry() const noexcept {
            return m_geometry;
        }

        template <typename TGeomHandler>
        void decode_point_geometry(TGeomHandler&& geom_handler) const;

        template <typename TGeomHandler>
        void decode_linestring_geometry(TGeomHandler&& geom_handler) const;

        template <typename TGeomHandler>
        void decode_polygon_geometry(TGeomHandler&& geom_handler) const;

        protozero::iterator_range<tags_iterator> tags(const layer& layer) const noexcept {
            return {{m_tags.begin(), m_tags.end(), &layer},
                    {m_tags.end(), m_tags.end(), &layer}};
        }

    }; // class feature

    /**
     * A layer according to spec 4.1
     */
    class layer {

        data_view m_data;
        uint32_t m_version;
        uint32_t m_extent;
        data_view m_name;
        protozero::pbf_message<detail::pbf_layer> m_layer_reader;
        std::vector<data_view> m_key_table;
        std::vector<data_view> m_value_table;

    public:

        layer() :
            m_data(),
            m_version(0),
            m_extent(0),
            m_name(),
            m_layer_reader() {
        }

        explicit layer(const data_view& data) :
            m_data(data),
            m_version(1), // defaults to 1, see https://github.com/mapbox/vector-tile-spec/blob/master/2.1/vector_tile.proto#L55
            m_extent(4096), // defaults to 4096, see https://github.com/mapbox/vector-tile-spec/blob/master/2.1/vector_tile.proto#L70
            m_name(),
            m_layer_reader(data) {
            protozero::pbf_message<detail::pbf_layer> reader{data};
            while (reader.next()) {
                switch (reader.tag_and_type()) {
                    case protozero::tag_and_type(detail::pbf_layer::version, protozero::pbf_wire_type::varint):
                        m_version = reader.get_uint32();
                        // This library can only handle version 1 and 2.
                        if (m_version < 1 || m_version > 2) {
                            throw version_exception{};
                        }
                        break;
                    case protozero::tag_and_type(detail::pbf_layer::name, protozero::pbf_wire_type::length_delimited):
                        m_name = reader.get_view();
                        break;
                    case protozero::tag_and_type(detail::pbf_layer::keys, protozero::pbf_wire_type::length_delimited):
                        m_key_table.push_back(reader.get_view());
                        break;
                    case protozero::tag_and_type(detail::pbf_layer::values, protozero::pbf_wire_type::length_delimited):
                        m_value_table.push_back(reader.get_view());
                        break;
                    case protozero::tag_and_type(detail::pbf_layer::extent, protozero::pbf_wire_type::varint):
                        m_extent = reader.get_uint32();
                        break;
                    default:
                        reader.skip();
                        break;
                }
            }

            // 4.1 "A layer MUST contain a name field."
            if (m_name.data() == nullptr) {
                throw format_exception{};
            }
        }

        bool valid() const noexcept {
            return m_version != 0;
        }

        operator bool() const noexcept {
            return valid();
        }

        data_view data() const noexcept {
            return m_data;
        }

        data_view name() const noexcept {
            assert(valid());
            return m_name;
        }

        std::uint32_t version() const noexcept {
            assert(valid());
            return m_version;
        }

        std::uint32_t extent() const noexcept {
            assert(valid());
            return m_extent;
        }

        const std::vector<data_view>& key_table() const noexcept {
            return m_key_table;
        }

        const std::vector<data_view>& value_table() const noexcept {
            return m_value_table;
        }

        const data_view& key(uint32_t n) const {
            if (n >= m_key_table.size()) {
                throw format_exception{};
            }

            return m_key_table[n];
        }

        const data_view& value(uint32_t n) const {
            if (n >= m_value_table.size()) {
                throw format_exception{};
            }

            return m_value_table[n];
        }

        /**
         * Reset feature iterator. The next call to get_next_feature() will
         * start at the first feature again.
         */
        void reset() {
            m_layer_reader = protozero::pbf_message<detail::pbf_layer>{m_data};
        }

        /**
         * Get the next feature in this layer.
         *
         * Complexity: Constant.
         * @returns Feature with the specified ID or the invalid feature if
         *          there are no more features in this layer.
         */
        feature get_next_feature() {
            assert(valid());
            if (m_layer_reader.next(detail::pbf_layer::features, protozero::pbf_wire_type::length_delimited)) {
                return feature{m_layer_reader.get_view()};
            }

            reset();
            return feature{};
        }

        /**
         * Get the feature with the specified ID.
         *
         * Complexity: Linear in the number of features.
         *
         * @param id The ID to look for.
         * @returns Feature with the specified ID or the invalid feature if
         *          there is no feature with this ID.
         */
        feature get_feature(uint64_t id) const {
            assert(valid());
            protozero::pbf_message<detail::pbf_layer> layer_reader{m_data};
            while (layer_reader.next(detail::pbf_layer::features, protozero::pbf_wire_type::length_delimited)) {
                const auto feature_data = layer_reader.get_view();
                protozero::pbf_message<detail::pbf_feature> feature_reader{feature_data};
                if (feature_reader.next(detail::pbf_feature::id, protozero::pbf_wire_type::varint)) {
                    if (feature_reader.get_uint64() == id) {
                        return feature{feature_data};
                    }
                }
            }

            return feature{};
        }

        /**
         * Count the number of features in this layer.
         *
         * Complexity: Linear in the number of features.
         */
        std::size_t get_feature_count() const {
            assert(valid());
            std::size_t count = 0;

            protozero::pbf_message<detail::pbf_layer> layer_reader{m_data};
            while (layer_reader.next(detail::pbf_layer::features, protozero::pbf_wire_type::length_delimited)) {
                layer_reader.skip();
                ++count;
            }

            return count;
        }

    }; // class layer

    class vector_tile {

        data_view m_data;
        protozero::pbf_message<detail::pbf_tile> m_tile_reader;

    public:

        explicit vector_tile(const data_view& data) :
            m_data(data),
            m_tile_reader(m_data) {
        }

        explicit vector_tile(const std::string& data) :
            m_data(data.data(), data.size()),
            m_tile_reader(m_data) {
        }

        void reset() {
            m_tile_reader = protozero::pbf_message<detail::pbf_tile>{m_data};
        }

        layer get_next_layer() {
            if (m_tile_reader.next(detail::pbf_tile::layers, protozero::pbf_wire_type::length_delimited)) {
                return layer{m_tile_reader.get_view()};
            }

            return layer{};
        }

        layer get_layer(std::size_t id) const {
            protozero::pbf_message<detail::pbf_tile> reader{m_data};

            while (reader.next(detail::pbf_tile::layers, protozero::pbf_wire_type::length_delimited)) {
                if (id == 0) {
                    return layer{reader.get_view()};
                }
                reader.skip();
                --id;
            }

            return layer{};
        }

        layer get_layer(const data_view& name) const {
            protozero::pbf_message<detail::pbf_tile> reader{m_data};

            while (reader.next(detail::pbf_tile::layers, protozero::pbf_wire_type::length_delimited)) {
                const auto layer_data = reader.get_view();
                protozero::pbf_message<detail::pbf_layer> layer_reader{layer_data};
                if (layer_reader.next(detail::pbf_layer::name, protozero::pbf_wire_type::length_delimited)) {
                    if (layer_reader.get_view() == name) {
                        return layer{layer_data};
                    }
                } else {
                    // 4.1 "A layer MUST contain a name field."
                    throw format_exception{};
                }
            }

            return layer{};
        }

    }; // class vector_tile

    template <typename TGeomHandler>
    void feature::decode_point_geometry(TGeomHandler&& geom_handler) const {
        assert(type() == vtzero::GeomType::POINT);

        geometry_decoder decoder{m_geometry, false};

        // spec 4.3.4.2 "MUST consist of of a single MoveTo command"
        if (!decoder.next_command(detail::command_move_to())) {
            throw geometry_exception{};
        }

        // spec 4.3.4.2 "command count greater than 0"
        if (decoder.count() == 0) {
            throw geometry_exception{};
        }

        while (decoder.count() > 0) {
            std::forward<TGeomHandler>(geom_handler).point(decoder.next_point());
        }

        // spec 4.3.4.2 "MUST consist of of a single ... command"
        if (!decoder.done()) {
            throw geometry_exception{};
        }
    }

    template <typename TGeomHandler>
    void feature::decode_linestring_geometry(TGeomHandler&& geom_handler) const {
        assert(type() == vtzero::GeomType::LINESTRING);

        geometry_decoder decoder{m_geometry, false};

        // spec 4.3.4.3 "1. A MoveTo command"
        while (decoder.next_command(detail::command_move_to())) {
            // spec 4.3.4.3 "with a command count of 1"
            if (decoder.count() != 1) {
                throw geometry_exception{};
            }

            std::forward<TGeomHandler>(geom_handler).linestring_begin();
            std::forward<TGeomHandler>(geom_handler).linestring_point(decoder.next_point());

            // spec 4.3.4.3 "2. A LineTo command"
            if (!decoder.next_command(detail::command_line_to())) {
                throw geometry_exception{};
            }

            // spec 4.3.4.3 "with a command count greater than 0"
            if (decoder.count() == 0) {
                throw geometry_exception{};
            }

            while (decoder.count() > 0) {
                std::forward<TGeomHandler>(geom_handler).linestring_point(decoder.next_point());
            }

            std::forward<TGeomHandler>(geom_handler).linestring_end();
        }
    }

    template <typename TGeomHandler>
    void feature::decode_polygon_geometry(TGeomHandler&& geom_handler) const {
        assert(type() == vtzero::GeomType::POLYGON);

        geometry_decoder decoder{m_geometry, false};

        // spec 4.3.4.4 "1. A MoveTo command"
        while (decoder.next_command(detail::command_move_to())) {
            // spec 4.3.4.4 "with a command count of 1"
            if (decoder.count() != 1) {
                throw geometry_exception{};
            }

            std::forward<TGeomHandler>(geom_handler).ring_begin();

            point start_point{decoder.next_point()};
            int64_t sum = 0;
            point last_point = start_point;

            std::forward<TGeomHandler>(geom_handler).ring_point(start_point);

            // spec 4.3.4.4 "2. A LineTo command"
            if (!decoder.next_command(detail::command_line_to())) {
                throw geometry_exception{};
            }

            // spec 4.3.4.4 "with a command count greater than 1"
/*            if (decoder.count() <= 1) {
                throw geometry_exception{};
            }*/

            while (decoder.count() > 0) {
                point p = decoder.next_point();
                sum += detail::det(last_point, p);
                last_point = p;
                std::forward<TGeomHandler>(geom_handler).ring_point(p);
            }

            // spec 4.3.4.4 "3. A ClosePath command"
            if (!decoder.next_command(detail::command_close_path())) {
                throw geometry_exception{};
            }

            // spec 4.3.3.3 "A ClosePath command MUST have a command count of 1"
            if (decoder.count() != 1) {
                throw geometry_exception{};
            }

            sum += detail::det(last_point, start_point);
            std::forward<TGeomHandler>(geom_handler).ring_point(start_point);

            std::forward<TGeomHandler>(geom_handler).ring_end(sum > 0);
        }
    }

    inline tag tags_iterator::operator*() const {
        assert(m_it != m_end);
        if (std::next(m_it) == m_end) {
            throw format_exception{};
        }
        return tag{m_layer->key(*m_it), m_layer->value(*std::next(m_it))};
    }

} // namespace vtzero

#endif // VTZERO_READER_HPP
