#include <mapbox/vector_tile.hpp>

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

std::string open_tile(std::string const& path) {
    std::ifstream stream(path.c_str(), std::ios_base::in|std::ios_base::binary);
    if (!stream.is_open())
    {
        throw std::runtime_error("could not open: '" + path + "'");
    }
    std::string message(std::istreambuf_iterator<char>(stream.rdbuf()), std::istreambuf_iterator<char>());
    stream.close();
    return message;
}

class print_value {
public:
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
            std::clog << "please provide path to uncompressed Mapbox Vector Tile (.mvt)\n";
            return -1;
        }
        std::string tile_path(argv[1]);
        std::string buffer = open_tile(tile_path);
        auto vt = mapbox::vector_tile::decode_tile<std::int64_t>(buffer);
        std::cout << "\nDecoding tile " << tile_path << " ...\n\n";
        for (auto const& layer : vt) {
            std::cout << "Layer \"" << layer.first << "\" (" << layer.second.size() << " features)\n";

            auto fc = vt[layer.first];

            for (std::size_t i = 0; i < layer.second.size(); ++i) {
                auto feature = fc[i];

                std::cout << "  feature (id: " << feature.id.get<std::uint64_t>() << ") (type: ";
                if (feature.geometry.is<mapbox::geometry::point<std::int64_t>>()) {
                    std::cout <<  "point) ";
                }
                if (feature.geometry.is<mapbox::geometry::line_string<std::int64_t>>()) {
                    std::cout <<  "linestring) ";
                }
                if (feature.geometry.is<mapbox::geometry::polygon<std::int64_t>>()) {
                    std::cout <<  "polygon) ";
                }

                std::cout << "properties:\n";
                for (auto const& prop : feature.properties) {
                    print_value printvisitor;
                    std::string value = mapbox::util::apply_visitor(printvisitor, prop.second);
                    std::cout << " - " << prop.first  << ": " << value << "\n";
                }
            }
        }
    } catch (std::exception const& ex) {
        std::clog << "ERROR: " << ex.what() << "\n";
        return -1;
    }
}
