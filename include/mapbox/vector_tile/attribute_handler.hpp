#pragma once

#include <mapbox/feature.hpp>
#include <vtzero/vector_tile.hpp>
#include <memory>

namespace mapbox {
namespace vector_tile {
namespace detail {

struct AttributeHandler
{

    using map_type = mapbox::feature::property_map;
    using vec_type = std::vector<mapbox::feature::value>;

    struct handler_store
    {
        std::unique_ptr<map_type> map;
        std::unique_ptr<vec_type> vec;
        vtzero::data_view last_key;
        vtzero::scaling const* scaling;
        bool is_map;
    };

    std::vector<handler_store> stack;
    vtzero::layer const& layer;
    handler_store* active;

    AttributeHandler(vtzero::layer const& layer_)
        : stack(),
          layer(layer_),
          active()
    {
        stack.push_back({std::make_unique<map_type>(), nullptr, {}, nullptr, true});
        active = &(stack.back());
    }

    map_type result()
    {
        return std::move(*(active->map));
    }

    vec_type vec_result()
    {
        return std::move(*(active->vec));
    }

    bool attribute_key(vtzero::data_view key, std::size_t /*depth*/)
    {
        active->last_key = key;
        return true;
    }

    bool attribute_value(vtzero::data_view value, std::size_t /*depth*/)
    {
        if (active->is_map)
        {
            active->map->emplace(std::string(active->last_key), mapbox::feature::value(std::string(value)));
        }
        else
        {
            active->vec->push_back(mapbox::feature::value(std::string(value)));
        }
        return true;
    }

    bool attribute_value(vtzero::null_type /*value*/, std::size_t /*depth*/)
    {
        if (active->is_map)
        {
            active->map->emplace(std::string(active->last_key), mapbox::feature::value(mapbox::feature::null_value_t()));
        }
        else
        {
            active->vec->push_back(mapbox::feature::value(mapbox::feature::null_value_t()));
        }
        return true;
    }

    bool attribute_value(int64_t value, std::size_t /*depth*/)
    {
        if (active->is_map)
        {
            active->map->emplace(std::string(active->last_key), mapbox::feature::value(value));
        }
        else
        {
            active->vec->push_back(mapbox::feature::value(value));
        }
        return true;
    }

    bool attribute_value(uint64_t value, std::size_t /*depth*/)
    {
        if (active->is_map)
        {
            active->map->emplace(std::string(active->last_key), mapbox::feature::value(value));
        }
        else
        {
            active->vec->push_back(mapbox::feature::value(value));
        }
        return true;
    }

    bool attribute_value(double value, std::size_t /*depth*/)
    {
        if (active->is_map)
        {
            active->map->emplace(std::string(active->last_key), mapbox::feature::value(value));
        }
        else
        {
            active->vec->push_back(mapbox::feature::value(value));
        }
        return true;
    }

    bool attribute_value(float value, std::size_t /*depth*/)
    {
        if (active->is_map)
        {
            active->map->emplace(std::string(active->last_key), mapbox::feature::value(static_cast<double>(value)));
        }
        else
        {
            active->vec->push_back(mapbox::feature::value(static_cast<double>(value)));
        }
        return true;
    }

    bool attribute_value(bool value, std::size_t /*depth*/)
    {
        if (active->is_map)
        {
            active->map->emplace(std::string(active->last_key), mapbox::feature::value(value));
        }
        else
        {
            active->vec->push_back(mapbox::feature::value(value));
        }
        return true;
    }

    bool start_list_attribute(std::size_t size, std::size_t /*depth*/)
    {
        stack.push_back({nullptr, std::make_unique<vec_type>(), {}, nullptr, false});
        active = &(stack.back());
        active->vec->reserve(size);
        return true;
    }

    bool end_list_attribute(std::size_t /*depth*/)
    {
        auto* prev_active = &*(++(stack.rbegin()));
        if (prev_active->is_map)
        {
            prev_active->map->emplace(std::string(prev_active->last_key), mapbox::feature::value(vec_result()));
        }
        else
        {
            prev_active->vec->push_back(mapbox::feature::value(vec_result()));
        }
        stack.pop_back();
        active = prev_active;
        return true;
    }

    bool start_map_attribute(std::size_t size, std::size_t /*depth*/)
    {
        stack.push_back({std::make_unique<map_type>(), nullptr, {}, nullptr, true});
        active = &(stack.back());
        active->map->reserve(size);
        return true;
    }

    bool end_map_attribute(std::size_t /*depth*/)
    {
        auto* prev_active = &*(++(stack.rbegin()));
        if (prev_active->is_map)
        {
            prev_active->map->emplace(std::string(prev_active->last_key), mapbox::feature::value(result()));
        }
        else
        {
            prev_active->vec->push_back(mapbox::feature::value(result()));
        }
        stack.pop_back();
        active = prev_active;
        return true;
    }

    bool start_number_list(std::size_t size, vtzero::index_value index, std::size_t /*depth*/)
    {
        auto const& scaling = layer.attribute_scaling(index);
        stack.push_back({nullptr, std::make_unique<vec_type>(), {}, &scaling, false});
        active = &(stack.back());
        active->vec->reserve(size);
        return true;
    }

    bool number_list_null_value(std::size_t /*depth*/)
    {
        active->vec->push_back(mapbox::feature::value(mapbox::feature::null_value_t()));
        return true;
    }

    bool number_list_value(int64_t value, std::size_t /*depth*/)
    {
        active->vec->push_back(mapbox::feature::value(active->scaling->decode(value)));
        return true;
    }

    bool end_number_list(std::size_t /*depth*/)
    {
        auto* prev_active = &*(++(stack.rbegin()));
        if (prev_active->is_map)
        {
            prev_active->map->emplace(std::string(prev_active->last_key), mapbox::feature::value(vec_result()));
        }
        else
        {
            prev_active->vec->push_back(mapbox::feature::value(vec_result()));
        }
        stack.pop_back();
        active = prev_active;
        return true;
    }
};

} // end ns detail
}
}
