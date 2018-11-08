#include <iostream>
#include <chrono>
#include <mapbox/vector_tile.hpp>
#include <fstream>

std::size_t feature_count = 0;

static void decode_entire_tile(std::string const& buffer) {
    mapbox::vector_tile::buffer tile(buffer);
    for (auto const& name : tile.layerNames()) {
        const mapbox::vector_tile::layer layer = tile.getLayer(name);
        std::size_t num_features = layer.featureCount();
        if (num_features == 0) {
            std::cout << "Layer '" << name << "' (empty)\n";
            continue;
        }
        for (std::size_t i=0;i<num_features;++i) {
            auto const feature = mapbox::vector_tile::feature(layer.getFeature(i),layer);
            auto const& feature_id = feature.getID();
            if (feature_id.is<mapbox::feature::null_value_t>()) {
                throw std::runtime_error("Hit unexpected error decoding feature");
            }
            auto props = feature.getProperties();
            mapbox::vector_tile::points_arrays_type geom = feature.getGeometries<mapbox::vector_tile::points_arrays_type>(1.0);
            ++feature_count;
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