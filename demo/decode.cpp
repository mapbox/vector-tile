#include <mapbox/vector_tile.hpp>
#include <fstream>
#include <stdexcept>
#include <iostream>

std::string open_tile(std::string const& path) {
    std::ifstream stream(path.c_str(),std::ios_base::in|std::ios_base::binary);
    if (!stream.is_open())
    {
        throw std::runtime_error("could not open: '" + path + "'");
    }
    std::string message(std::istreambuf_iterator<char>(stream.rdbuf()),(std::istreambuf_iterator<char>()));
    stream.close();
    return message;
}

class print_value {

public:
    std::string operator()(std::vector<mapbox::feature::value> val) {
        return "vector";
    }

    std::string operator()(std::unordered_map<std::string, mapbox::feature::value> val) {
        return "unordered_map";
    }

    std::string operator()(mapbox::feature::null_value_t val) {
        return "null";
    }

    std::string operator()(std::nullptr_t val) {
        return "nullptr";
    }

    std::string operator()(uint64_t val) {
        return std::to_string(val);
    }
    std::string operator()(int64_t val) {
        return std::to_string(val);
    }
    std::string operator()(double val) {
        return std::to_string(val);
    }
    std::string operator()(std::string const& val) {
        return val;
    }

    std::string operator()(bool val) {
        return val ? "true" : "false";
    }
};

int main(int argc, char** argv) {
    try {
        if (argc < 2) {
            std::clog << "please provide path to uncompressed Mapbox Vector tile (.mvt)\n";
            return -1;
        }
        std::string tile_path(argv[1]);
        std::string buffer = open_tile(tile_path);
        mapbox::vector_tile::buffer tile(buffer);
        std::cout << "Decoding tile: " << tile_path << "\n";
        for (auto const& name : tile.layerNames()) {
            const mapbox::vector_tile::layer layer = tile.getLayer(name);
            std::size_t feature_count = layer.featureCount();
            if (feature_count == 0) {
                std::cout << "Layer '" << name << "' (empty)\n";
                continue;
            }
            std::cout << "Layer '" << name << "'\n";
            std::cout << "  Features:\n";
            for (std::size_t i=0;i<feature_count;++i) {
                auto const feature = mapbox::vector_tile::feature(layer.getFeature(i),layer);
                auto const& feature_id = feature.getID();
                if (feature_id.is<uint64_t>()) {
                    std::cout << "    id: " << feature_id.get<uint64_t>() << "\n";
                } else {
                    std::cout << "    id: (no id set)\n";
                }
                std::cout << "    type: " << int(feature.getType()) << "\n";
                auto props = feature.getProperties();
                std::cout << "    Properties:\n";
                for (auto const& prop : props) {
                    print_value printvisitor;
                    std::string value = mapbox::util::apply_visitor(printvisitor, prop.second);
                    std::cout << "      " << prop.first  << ": " << value << "\n";
                }
                std::cout << "    Vertices:\n";
                mapbox::vector_tile::points_arrays_type geom = feature.getGeometries<mapbox::vector_tile::points_arrays_type>(1.0);
                for (auto const& point_array : geom) {
                    for (auto const& point : point_array) {
                        std::clog << point.x << "," << point.y;
                    }
                }
                std::clog << "\n";
            }
        }
    } catch (std::exception const& ex) {
        std::clog << "ERROR: " << ex.what() << "\n";
        return -1;
    }
}