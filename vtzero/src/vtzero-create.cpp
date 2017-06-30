
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <vtzero/builder.hpp>

int main() {
    vtzero::tile_builder tile;
    auto& layer_points = tile.add_layer("points");
    auto& layer_lines = tile.add_layer("lines");

    {
        vtzero::point_feature_builder feature{layer_points, 1 /* id */, {10, 10}};
        feature.add_attribute("foo", "bar");
        feature.add_attribute("x", "y");
    }
    {
        vtzero::point_feature_builder feature{layer_points, 2 /* id */, {20, 20}};
        feature.add_attribute("some", "attr");
    }
    {
        vtzero::line_string_feature_builder feature{layer_lines, 3 /* id */};
        feature.add_attribute("highway", "primary");
        feature.add_attribute("maxspeed", "50");
        feature.start_linestring(3);
        feature.add_point({10, 10});
        feature.add_point({10, 20});
        feature.add_point({20, 20});
        feature.start_linestring(2);
        feature.add_point({11, 11});
        feature.add_point({12, 13});
    }

    const auto data = tile.serialize();
    const int fd = ::open("test.mvt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    assert(fd > 0);

    const auto len = ::write(fd, data.c_str(), data.size());
    assert(static_cast<size_t>(len) == data.size());
}

