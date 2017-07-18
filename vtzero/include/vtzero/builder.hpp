#ifndef VTZERO_BUILDER_HPP
#define VTZERO_BUILDER_HPP

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <protozero/pbf_builder.hpp>

#include "geometry.hpp"
#include "reader.hpp"
#include "types.hpp"

namespace vtzero {

    class layer_builder_base {

    public:

        virtual void build(protozero::pbf_builder<detail::pbf_tile>& pbf_tile_builder) = 0;

    }; // class layer_builder_base

    struct kv_index {
        uint32_t k;
        uint32_t v;
    };

    class layer_builder : public layer_builder_base {

        std::string m_data;
        std::string m_keys_data;
        std::string m_values_data;
        std::map<std::string, uint32_t> m_keys_map;
        std::map<std::string, uint32_t> m_values_map;

        protozero::pbf_builder<detail::pbf_layer> m_pbf_message_layer;
        protozero::pbf_builder<detail::pbf_layer> m_pbf_message_keys;
        protozero::pbf_builder<detail::pbf_layer> m_pbf_message_values;

        uint32_t m_max_key = 0;
        uint32_t m_max_value = 0;

    public:

        template <typename T>
        layer_builder(T&& name, uint32_t version, uint32_t extent) :
            layer_builder_base(),
            m_data(),
            m_keys_data(),
            m_values_data(),
            m_keys_map(),
            m_values_map(),
            m_pbf_message_layer(m_data),
            m_pbf_message_keys(m_keys_data),
            m_pbf_message_values(m_values_data) {
            m_pbf_message_layer.add_uint32(detail::pbf_layer::version, version);
            m_pbf_message_layer.add_string(detail::pbf_layer::name, std::forward<T>(name));
            m_pbf_message_layer.add_uint32(detail::pbf_layer::extent, extent);
        }

        uint32_t add_key(const data_view& text) {
            auto p = m_keys_map.insert(std::make_pair(std::string{text.data(), text.size()}, m_max_key));

            if (p.second) {
                ++m_max_key;
                m_pbf_message_keys.add_string(detail::pbf_layer::keys, text);
            }

            return p.first->second;
        }

        uint32_t add_value(const data_view& text) {
            auto p = m_values_map.insert(std::make_pair(std::string{text.data(), text.size()}, m_max_value));

            if (p.second) {
                ++m_max_value;

                m_pbf_message_values.add_string(detail::pbf_layer::values, text);
            }

            return p.first->second;
        }

        kv_index add_tag(const data_view& key, const data_view& value) {
            return {add_key(key), add_value(value)};
        }

        const std::string& data() const noexcept {
            return m_data;
        }

        const std::string& keys_data() const noexcept {
            return m_keys_data;
        }

        const std::string& values_data() const noexcept {
            return m_values_data;
        }

        protozero::pbf_builder<detail::pbf_layer>& message() noexcept {
            return m_pbf_message_layer;
        }

        void add_feature(feature& feature, layer& layer);

        void build(protozero::pbf_builder<detail::pbf_tile>& pbf_tile_builder) override {
            pbf_tile_builder.add_bytes_vectored(detail::pbf_tile::layers,
                data(),
                keys_data(),
                values_data()
            );
        }

    }; // class layer_builder

    class layer_builder_existing : public layer_builder_base {

        data_view m_data;

    public:

        layer_builder_existing(const data_view& data) :
            layer_builder_base(),
            m_data(data) {
        }

        void build(protozero::pbf_builder<detail::pbf_tile>& pbf_tile_builder) override {
            pbf_tile_builder.add_bytes(detail::pbf_tile::layers, m_data.data(), m_data.size());
        }

    }; // class layer_builder_existing

    class feature_builder {

        layer_builder& m_layer;

        void add_tag_internal(uint32_t key_idx, const data_view value) {
            m_pbf_tags.add_element(key_idx);
            m_pbf_tags.add_element(m_layer.add_value(value));
        }

        void add_tag_internal(const data_view key, const data_view value) {
            const auto idx = m_layer.add_tag(key, value);
            m_pbf_tags.add_element(idx.k);
            m_pbf_tags.add_element(idx.v);
        }

    protected:

        protozero::pbf_builder<detail::pbf_feature> m_feature_writer;
        protozero::packed_field_uint32 m_pbf_tags;

        feature_builder(layer_builder& layer, uint64_t id) :
            m_layer(layer),
            m_feature_writer(layer.message(), detail::pbf_layer::features) {
            m_feature_writer.add_uint64(detail::pbf_feature::id, id);
        }

        void add_tag_impl(const tag_view& tag) {
            add_tag_internal(tag.key(), tag.value().data());
        }

        template <typename TValue>
        void add_tag_impl(uint32_t key_idx, TValue&& value) {
            tag_value v{std::forward<TValue>(value)};
            add_tag_internal(key_idx, v.data());
        }

        template <typename TKey, typename TValue,
                  typename std::enable_if<!std::is_same<typename std::decay<TKey>::type, uint32_t>{}, int>::type = 0>
        void add_tag_impl(TKey&& key, TValue&& value) {
            data_view k{std::forward<TKey>(key)};
            tag_value v{std::forward<TValue>(value)};
            add_tag_internal(k, v.data());
        }

        void do_commit() {
            if (m_pbf_tags.valid()) {
                m_pbf_tags.commit();
            }
            m_feature_writer.commit();
        }

        void do_rollback() {
            if (m_pbf_tags.valid()) {
                m_pbf_tags.commit();
            }
            m_feature_writer.rollback();
        }

    }; // class feature_builder

    class geometry_feature_builder : public feature_builder {

    public:

        geometry_feature_builder(layer_builder& layer, uint64_t id, GeomType geom_type, const data_view& geometry) :
            feature_builder(layer, id) {
            m_feature_writer.add_enum(detail::pbf_feature::type, static_cast<int32_t>(geom_type));
            m_feature_writer.add_string(detail::pbf_feature::geometry, geometry);
            m_pbf_tags = {m_feature_writer, detail::pbf_feature::tags};
        }

        template <typename ...TArgs>
        void add_tag(TArgs&& ...args) {
            add_tag_impl(std::forward<TArgs>(args)...);
        }

    }; // class geometry_feature_builder

    class point_feature_builder : public feature_builder {

        protozero::packed_field_uint32 m_pbf_geometry{};
        size_t m_num_points = 0;
        point m_cursor{0, 0};

    public:

        point_feature_builder(layer_builder& layer, uint64_t id = 0) :
            feature_builder(layer, id) {
            m_feature_writer.add_enum(detail::pbf_feature::type, static_cast<int32_t>(GeomType::POINT));
            m_pbf_geometry = {m_feature_writer, detail::pbf_feature::geometry};
        }

        ~point_feature_builder() {
            assert(m_num_points == 0 && "has fewer points than expected");
        }

        void points_begin(uint32_t count) {
            assert(m_pbf_geometry.valid());
            assert(!m_pbf_tags.valid());
            m_num_points = count;
            m_pbf_geometry.add_element(detail::command_move_to(count));
        }

        void add_point(const point p) {
            assert(m_pbf_geometry.valid());
            assert(!m_pbf_tags.valid() && "Call points_begin() before add_point()");
            assert(m_num_points > 0 && "Too many calls to add_point()");
            --m_num_points;
            m_pbf_geometry.add_element(protozero::encode_zigzag32(p.x - m_cursor.x));
            m_pbf_geometry.add_element(protozero::encode_zigzag32(p.y - m_cursor.y));
            m_cursor = p;
        }

        void points_end() {
            assert(m_pbf_geometry.valid());
            assert(m_num_points == 0 && "Not enough calls to add_point()");
            m_pbf_geometry.commit();
            m_pbf_tags = {m_feature_writer, detail::pbf_feature::tags};
        }

        void add_single_point(const point p) {
            assert(m_pbf_geometry.valid());
            m_pbf_geometry.add_element(detail::command_move_to(1));
            m_pbf_geometry.add_element(protozero::encode_zigzag32(p.x));
            m_pbf_geometry.add_element(protozero::encode_zigzag32(p.y));
            points_end();
        }

        template <typename ...TArgs>
        void add_tag(TArgs&& ...args) {
            assert(m_pbf_tags.valid() && "Add point(s) before adding tags");
            add_tag_impl(std::forward<TArgs>(args)...);
        }

        void commit() {
            if (m_pbf_geometry.valid()) {
                m_pbf_geometry.commit();
            }
            do_commit();
        }

        void rollback() {
            if (m_pbf_geometry.valid()) {
                m_pbf_geometry.commit();
            }
            do_rollback();
        }

    }; // class point_feature_builder

    class line_string_feature_builder : public feature_builder {

        protozero::packed_field_uint32 m_pbf_geometry{};
        size_t m_num_points = 0;
        point m_cursor{0, 0};
        bool m_start_line = false;

    public:

        line_string_feature_builder(layer_builder& layer, uint64_t id = 0) :
            feature_builder(layer, id) {
            m_feature_writer.add_enum(detail::pbf_feature::type, static_cast<int32_t>(GeomType::LINESTRING));
            m_pbf_geometry = {m_feature_writer, detail::pbf_feature::geometry};
        }

        ~line_string_feature_builder() {
            assert(m_num_points == 0 && "LineString has fewer points than expected");
        }

        template <typename ...TArgs>
        void add_tag(TArgs&& ...args) {
            if (m_pbf_geometry.valid()) {
                m_pbf_geometry.commit();
                m_pbf_tags = {m_feature_writer, detail::pbf_feature::tags};
            }
            add_tag_impl(std::forward<TArgs>(args)...);
        }

        void linestring_begin(size_t count) {
            assert(m_pbf_geometry.valid());
            assert(!m_pbf_tags.valid());
            assert(count > 1);
            assert(m_num_points == 0 && "LineString has fewer points than expected");
            m_num_points = count;
            m_start_line = true;
        }

        void add_point(const point p) {
            assert(m_pbf_geometry.valid());
            assert(!m_pbf_tags.valid() && "Call linestring_begin() before add_point()");
            assert(m_num_points > 0 && "Too many calls to add_point()");
            --m_num_points;
            if (m_start_line) {
                m_pbf_geometry.add_element(detail::command_move_to(1));
                m_pbf_geometry.add_element(protozero::encode_zigzag32(p.x - m_cursor.x));
                m_pbf_geometry.add_element(protozero::encode_zigzag32(p.y - m_cursor.y));
                m_pbf_geometry.add_element(detail::command_line_to(m_num_points));
                m_start_line = false;
            } else {
                assert(p != m_cursor);
                m_pbf_geometry.add_element(protozero::encode_zigzag32(p.x - m_cursor.x));
                m_pbf_geometry.add_element(protozero::encode_zigzag32(p.y - m_cursor.y));
            }
            m_cursor = p;
        }

        void linestring_end() {
            assert(m_pbf_geometry.valid());
            assert(m_num_points == 0 && "Not enough calls to add_point()");
        }

    }; // class line_string_feature_builder

    class polygon_feature_builder : public feature_builder {

        protozero::packed_field_uint32 m_pbf_geometry{};
        size_t m_num_points = 0;
        point m_cursor{0, 0};
        point m_first_point{0, 0};
        bool m_start_ring = false;

    public:

        polygon_feature_builder(layer_builder& layer, uint64_t id = 0) :
            feature_builder(layer, id) {
            m_feature_writer.add_enum(detail::pbf_feature::type, static_cast<int32_t>(GeomType::POLYGON));
            m_pbf_geometry = {m_feature_writer, detail::pbf_feature::geometry};
        }

        ~polygon_feature_builder() {
            assert(m_num_points == 0 && "ring has fewer points than expected");
        }

        template <typename ...TArgs>
        void add_tag(TArgs&& ...args) {
            if (m_pbf_geometry.valid()) {
                m_pbf_geometry.commit();
                m_pbf_tags = {m_feature_writer, detail::pbf_feature::tags};
            }
            add_tag_impl(std::forward<TArgs>(args)...);
        }

        void ring_begin(size_t count) {
            assert(m_pbf_geometry.valid());
            assert(!m_pbf_tags.valid());
            assert(count > 3);
            assert(m_num_points == 0 && "ring has fewer points than expected");
            m_num_points = count;
            m_start_ring = true;
        }

        void add_point(const point p) {
            assert(m_pbf_geometry.valid());
            assert(!m_pbf_tags.valid() && "Call ring_begin() before add_point()");
            assert(m_num_points > 0 && "Too many calls to add_point()");
            --m_num_points;
            if (m_start_ring) {
                m_first_point = p;
                m_pbf_geometry.add_element(detail::command_move_to(1));
                m_pbf_geometry.add_element(protozero::encode_zigzag32(p.x - m_cursor.x));
                m_pbf_geometry.add_element(protozero::encode_zigzag32(p.y - m_cursor.y));
                m_pbf_geometry.add_element(detail::command_line_to(m_num_points - 1));
                m_start_ring = false;
                m_cursor = p;
            } else if (m_num_points == 0) {
                assert(m_first_point == p); // XXX
                // spec 4.3.3.3 "A ClosePath command MUST have a command count of 1"
                m_pbf_geometry.add_element(detail::command_close_path(1));
            } else {
                assert(p != m_cursor); // XXX
                m_pbf_geometry.add_element(protozero::encode_zigzag32(p.x - m_cursor.x));
                m_pbf_geometry.add_element(protozero::encode_zigzag32(p.y - m_cursor.y));
                m_cursor = p;
            }
        }

        void ring_end() {
            assert(m_pbf_geometry.valid());
            assert(m_num_points == 0 && "Not enough calls to add_point()");
        }

    }; // class line_string_feature_builder

    inline void layer_builder::add_feature(feature& feature, layer& layer) {
        geometry_feature_builder feature_builder{*this, feature.id(), feature.type(), feature.geometry()};
        for (auto tag : feature.tags(layer)) {
            feature_builder.add_tag(tag);
        }
    }

    class tile_builder {

        std::vector<std::unique_ptr<layer_builder_base>> m_layers;

    public:

        layer_builder& add_layer(const layer& layer) {
            m_layers.emplace_back(new layer_builder{layer.name(), layer.version(), layer.extent()});
            return *static_cast<layer_builder*>(m_layers.back().get());
        }

        template <typename T,
                  typename std::enable_if<!std::is_same<typename std::decay<T>::type, layer>{}, int>::type = 0>
        layer_builder& add_layer(T&& name, uint32_t version = 2, uint32_t extent = 4096) {
            m_layers.emplace_back(new layer_builder{std::forward<T>(name), version, extent});
            return *static_cast<layer_builder*>(m_layers.back().get());
        }

        void add_layer_with_data(const layer& layer) {
            m_layers.emplace_back(new layer_builder_existing{layer.data()});
        }

        std::string serialize() {
            std::string data;

            protozero::pbf_builder<detail::pbf_tile> pbf_tile_builder{data};
            for (auto& layer : m_layers) {
                layer->build(pbf_tile_builder);
            }

            return data;
        }

        void serialize(std::string& data) {
            protozero::pbf_builder<detail::pbf_tile> pbf_tile_builder{data};
            for (auto& layer : m_layers) {
                layer->build(pbf_tile_builder);
            }
        }

    }; // class tile_builder

} // namespace vtzero

#endif // VTZERO_BUILDER_HPP
