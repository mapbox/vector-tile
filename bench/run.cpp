#include <iostream>
#include <chrono>
#include <cmath>
#include <mapbox/vector_tile.hpp>
#include <mapbox/geometry.hpp>
#include <fstream>
#include <string>
#include <vector>

#include <vtzero/reader.hpp>

std::size_t feature_count = 0;

struct set_value {
    mapbox::geometry::value val;

    template <typename T>
    void operator()(const T& v) {
        val = v;
    }

    void operator()(const protozero::data_view& v) {
        val = std::string{v};
    }

    void operator()(const float v) {
        val = double(v);
    }
};

mapbox::geometry::property_map get_properties(const vtzero::feature& feature, const vtzero::layer& layer) {
    mapbox::geometry::property_map props;

    const auto tags = feature.tags(layer);
    props.reserve(tags.size());

    for (const auto& tag : tags) {
        set_value sv;
        vtzero::value_visit(sv, tag.value());
        props.emplace(tag.key().to_string(), std::move(sv.val));
    }

    return props;
}

struct geom_handler_points {

    mapbox::vector_tile::points_arrays_type& m_paths;
    float m_scale;

    geom_handler_points(mapbox::vector_tile::points_arrays_type& paths, float scale) :
        m_paths(paths),
        m_scale(scale) {
    }

    void points_begin(uint32_t count) const noexcept {
        m_paths.emplace_back();
        m_paths.back().reserve(count);
    }

    void points_point(const vtzero::point point) const {
        const float x = std::round(static_cast<float>(point.x) * m_scale);
        const float y = std::round(static_cast<float>(point.y) * m_scale);
        m_paths.back().emplace_back(x, y);
    }

    void points_end() const noexcept {
    }

};

struct geom_handler_linestrings {

    mapbox::vector_tile::points_arrays_type& m_paths;
    float m_scale;

    geom_handler_linestrings(mapbox::vector_tile::points_arrays_type& paths, float scale) :
        m_paths(paths),
        m_scale(scale) {
    }

    void linestring_begin(uint32_t count) const noexcept {
        m_paths.emplace_back();
        m_paths.back().reserve(count);
    }

    void linestring_point(const vtzero::point point) const noexcept {
        const float x = std::round(static_cast<float>(point.x) * m_scale);
        const float y = std::round(static_cast<float>(point.y) * m_scale);
        m_paths.back().emplace_back(x, y);
    }

    void linestring_end() const noexcept {
    }

};

struct geom_handler_polygons {

    mapbox::vector_tile::points_arrays_type& m_paths;
    float m_scale;

    geom_handler_polygons(mapbox::vector_tile::points_arrays_type& paths, float scale) :
        m_paths(paths),
        m_scale(scale) {
    }

    void ring_begin(uint32_t count) const noexcept {
        m_paths.emplace_back();
        m_paths.back().reserve(count);
    }

    void ring_point(const vtzero::point point) const noexcept {
        const float x = std::round(static_cast<float>(point.x) * m_scale);
        const float y = std::round(static_cast<float>(point.y) * m_scale);
        m_paths.back().emplace_back(x, y);
    }

    void ring_end(bool /*is_outer*/) const noexcept {
    }

};


mapbox::vector_tile::points_arrays_type get_geometry(const vtzero::feature& feature, float scale) {
    mapbox::vector_tile::points_arrays_type paths;

    bool strict = false;

    switch (feature.type()) {
        case vtzero::GeomType::POINT:
            vtzero::decode_point_geometry(feature.geometry(), strict, geom_handler_points{paths, scale});
            break;
        case vtzero::GeomType::LINESTRING:
            vtzero::decode_linestring_geometry(feature.geometry(), strict, geom_handler_linestrings{paths, scale});
            break;
        case vtzero::GeomType::POLYGON:
            vtzero::decode_polygon_geometry(feature.geometry(), strict, geom_handler_polygons{paths, scale});
            break;
        default:
            std::cout << "UNKNOWN GEOMETRY TYPE\n";
    }

    return paths;
}

static void decode_entire_tile(std::string const& buffer) {
    vtzero::vector_tile tile{buffer};
    std::ostream cnull(0);
    for (const auto layer : tile) {
        if (layer.get_feature_count() == 0) {
            std::cout << "Layer '" << std::string{layer.name()} << "' (empty)\n";
            continue;
        }
        for (const auto feature : layer) {
            if (!feature.has_id()) {
                throw std::runtime_error("Hit unexpected error decoding feature");
            }
            auto const feature_id = feature.id();
            (void)feature_id;
            auto props = get_properties(feature, layer);
/*            for (const auto x : props) {
                std::cout << x.first << ':';
                x.second.match([](mapbox::geometry::null_value_t) { std::cout << "NULL"; },
                               [](bool r){ std::cout << r; },
                               [](int64_t r){ std::cout << r; },
                               [](uint64_t r){ std::cout << r; },
                               [](double r){ std::cout << r; },
                               [](std::string r){ std::cout << r; },
                               [](auto){ std::cout << "XXX"; });
                std::cout << '\n';
            }*/
            mapbox::vector_tile::points_arrays_type geom = get_geometry(feature, 1.0);
            ++feature_count;
            cnull << props.size() << geom.size();
        }
    }
}

static void run_bench(std::vector<std::string> const& tiles, std::size_t iterations) {

    for (std::size_t i=0;i<iterations;++i) {
        for (auto const& tile: tiles) {
            decode_entire_tile(tile);
        }
    }
}

template <typename T>
using milliseconds = std::chrono::duration<T, std::milli>;

int main(/*int argc, char* const argv[]*/) {
    try {
        std::vector<std::string> tiles;
        for (std::size_t x=4680;x<=4693;++x) {
            for (std::size_t y=6260;y<=6274;++y) {
                std::string path = "bench/mvt-bench-fixtures/fixtures/14-" + std::to_string(x) + "-" + std::to_string(y) + ".mvt";
                std::ifstream stream(path.c_str(),std::ios_base::in|std::ios_base::binary);
                if (!stream.is_open())
                {
                    throw std::runtime_error("could not open: '" + path + "'");
                }
                std::string message(std::istreambuf_iterator<char>(stream.rdbuf()),(std::istreambuf_iterator<char>()));
                stream.close();
                tiles.emplace_back(message);
            }
        }
        std::clog << "decoding " << tiles.size() << " tiles\n";
        std::clog << "warming up...\n";
        // warmup
        run_bench(tiles,1);
        std::clog << "running bench...\n";
        // now actually decode all tiles for N iterations
        auto t1 = std::chrono::high_resolution_clock::now();
        run_bench(tiles,100);
        auto t2 = std::chrono::high_resolution_clock::now();
        auto elapsed = milliseconds<double>(t2 - t1).count();
        if (feature_count != 8157770) {
            std::clog << "Warning expected feature_count of 8157770, was: " << feature_count << "\n";
        }
        std::clog << "elapsed: " << std::fixed << elapsed << " ms\n";
    } catch (std::exception const& ex) {
        std::cerr << ex.what() << "\n";
        return -1;
    }
    return 0;
}
