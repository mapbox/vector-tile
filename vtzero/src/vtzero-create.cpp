
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <vtzero/builder.hpp>

int main() {
    vtzero::tile_builder tile;
    auto& layer_points   = tile.add_layer("points");
    auto& layer_lines    = tile.add_layer("lines");
    auto& layer_polygons = tile.add_layer("polygons");

    {
        vtzero::point_feature_builder feature{layer_points, 1 /* id */};
        feature.points_begin(1);
        feature.add_point({10, 10});
        feature.points_end();
        feature.add_tag("foo", "bar");
        feature.add_tag("x", "y");
        feature.rollback();
    }

    vtzero::point_feature_builder feature{layer_points, 2 /* id */};
    feature.add_single_point({20, 20});
    feature.add_tag("some", "attr");
    feature.commit();

    {
        vtzero::line_string_feature_builder feature{layer_lines, 3 /* id */};
        feature.linestring_begin(3);
        feature.add_point({10, 10});
        feature.add_point({10, 20});
        feature.add_point({20, 20});
        feature.linestring_end();
        feature.linestring_begin(2);
        feature.add_point({11, 11});
        feature.add_point({12, 13});
        feature.linestring_end();
        feature.add_tag("highway", "primary");
        feature.add_tag(std::string{"maxspeed"}, vtzero::sint_value(50));
    }

    {
        vtzero::polygon_feature_builder feature{layer_polygons, 4 /* id */};
        feature.ring_begin(5);
        feature.add_point({0, 0});
        feature.add_point({10, 0});
        feature.add_point({10, 10});
        feature.add_point({0, 10});
        feature.add_point({0, 0});
        feature.ring_end();
        feature.ring_begin(4);
        feature.add_point({3, 3});
        feature.add_point({3, 5});
        feature.add_point({5, 5});
        feature.add_point({3, 3});
        feature.ring_end();
        feature.add_tag("natural", "wood");
    }

    const auto data = tile.serialize();
    const int fd = ::open("test.mvt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    assert(fd > 0);

    const auto len = ::write(fd, data.c_str(), data.size());
    assert(static_cast<size_t>(len) == data.size());
}

