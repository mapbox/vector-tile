#include <vector_tile.hpp>

#include <iostream>
#include <cassert>

using namespace vector_tile;

static void test_config() {
    assert(Encoding::Tile::LAYERS == 3);

    assert(Encoding::Layer::VERSION == 15);
    assert(Encoding::Layer::NAME == 1);
    assert(Encoding::Layer::FEATURES == 2);
    assert(Encoding::Layer::KEYS == 3);
    assert(Encoding::Layer::VALUES == 4);
    assert(Encoding::Layer::EXTENT == 5);

    assert(Encoding::Feature::ID == 1);
    assert(Encoding::Feature::TAGS == 2);
    assert(Encoding::Feature::TYPE == 3);
    assert(Encoding::Feature::GEOMETRY == 4);
    assert(Encoding::Feature::RASTER == 5);

    assert(Encoding::Value::STRING == 1);
    assert(Encoding::Value::FLOAT == 2);
    assert(Encoding::Value::DOUBLE == 3);
    assert(Encoding::Value::INT == 4);
    assert(Encoding::Value::UINT == 5);
    assert(Encoding::Value::SINT == 6);
    assert(Encoding::Value::BOOL == 7);

    assert(Encoding::GeomType::UNKNOWN == 0);
    assert(Encoding::GeomType::POINT == 1);
    assert(Encoding::GeomType::LINESTRING == 2);
    assert(Encoding::GeomType::POLYGON == 3);
}

int main() {
    test_config();
    std::cout << "Success!\n";
    return 0;
}