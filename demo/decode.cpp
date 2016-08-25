#include <mapbox/vector_tile.hpp>
#include <fstream>
#include <stdexcept>

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
    std::string operator()(std::vector<mapbox::geometry::value> val) {
        return "vector";
    }

    std::string operator()(std::unordered_map<std::string, mapbox::geometry::value> val) {
        return "unordered_map";
    }

    std::string operator()(mapbox::geometry::null_value_t val) {
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
            auto layer_ptr = tile.getLayer(name);
            if (layer_ptr == nullptr) {
                throw std::runtime_error("Hit unexpected error decoding layer");
            }
            const mapbox::vector_tile::layer & layer = *layer_ptr;
            std::size_t feature_count = layer.featureCount();
            if (feature_count == 0) {
                std::cout << "Layer '" << name << "' (empty)\n";
                continue;
            }
            std::cout << "Layer '" << name << "'\n";
            std::cout << "  Features:\n";
            for (std::size_t i=0;i<feature_count;++i) {
                auto feature_ptr = layer.getFeature(i);
                if (feature_ptr == nullptr) {
                    throw std::runtime_error("Hit unexpected error decoding feature");
                }
                const mapbox::vector_tile::feature & feature = * feature_ptr;
                auto feature_id = feature.getID();
                if (!feature_id || !feature_id->is<uint64_t>()) {
                    throw std::runtime_error("Hit unexpected error decoding feature");
                }
                std::cout << "    id: " << feature_id->get<uint64_t>() << "\n";
                std::cout << "    type: " << int(feature.getType()) << "\n";
                auto props = feature.getProperties();
                std::cout << "    Properties:\n";
                for (auto const& prop : props) {
                    print_value printvisitor;
                    std::string value = mapbox::util::apply_visitor(printvisitor, prop.second);
                    std::cout << "      " << prop.first  << ": " << value << "\n";
                }
            }
        }
    } catch (std::exception const& ex) {
        std::clog << "ERROR: " << ex.what() << "\n";
        return -1;
    }
}