
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <vtzero/builder.hpp>

int main() {
    vtzero::tile_builder tile;
    auto& layer_points      = tile.add_layer("points");
    auto& layer_lines       = tile.add_layer("lines");
    auto& layer_polygons    = tile.add_layer("polygons");
    auto& layer_splines     = tile.add_layer("splines");
    auto& layer_points_3d   = tile.add_layer("points_3d", 2, 4096, 3);
    auto& layer_lines_3d    = tile.add_layer("lines_3d", 2, 4096, 3);
    auto& layer_polygons_3d = tile.add_layer("polygons_3d", 2, 4096, 3);
    auto& layer_splines_3d  = tile.add_layer("splines_3d", 2, 4096, 3);

    {
        vtzero::point_feature_builder<2> feature{layer_points, 1 /* id */};
        feature.points_begin(1);
        feature.add_point({10, 10});
        feature.points_end();
        feature.add_tag("foo", "bar");
        feature.add_tag("x", "y");
        feature.rollback();
    }

    {
        vtzero::point_feature_builder<2> feature{layer_points, 2 /* id */};
        feature.add_single_point({20, 20});
        feature.add_tag("some", "attr");
    }
    {
        vtzero::point_feature_builder<2> feature{layer_points, 3 /* id */};
        feature.add_single_point({20, 20});
        feature.add_tag("some", "attr");
    }

    {
        vtzero::point_feature_builder<2> feature{layer_points, 4 /* id */};
        feature.add_single_point({20, 20});
        feature.add_tag("some", "otherattr");
    }

    {
        vtzero::point_feature_builder<2> feature{layer_points, 5 /* id */};
        feature.add_single_point({20, 20});
        feature.add_tag("otherkey", "attr");
        feature.commit();
    }

    {
        vtzero::line_string_feature_builder<2> feature{layer_lines, 6 /* id */};
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
        vtzero::polygon_feature_builder<2> feature{layer_polygons, 7 /* id */};
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
    {
        vtzero::spline_feature_builder<2> feature{layer_splines, 8 /* id */};
        // Add control points
        feature.controlpoints_begin(4);
        feature.add_point({8, 10});
        feature.add_point({9, 11});
        feature.add_point({11, 9});
        feature.add_point({12, 10});
        feature.controlpoints_end();
        // Add knots
        feature.knots_begin();
        feature.add_knot(0.0);
        feature.add_knot(0.0);
        feature.add_knot(0.0);
        feature.add_knot(1.0);
        feature.add_knot(2.0);
        feature.add_knot(2.0);
        feature.add_knot(2.0);
        feature.knots_end();
        // Add tags
        feature.tags_begin();
        feature.add_tag("natural", "curve");
        feature.tags_end();
    }
    // 3d data
    {
        vtzero::point_feature_builder<3> feature{layer_points_3d, 9 /* id */};
        feature.points_begin(1);
        feature.add_point({10, 10, 10});
        feature.points_end();
        feature.add_tag("foo", "bar");
        feature.add_tag("x", "y");
        feature.rollback();
    }

    {
        vtzero::point_feature_builder<3> feature{layer_points_3d, 10 /* id */};
        feature.add_single_point({20, 20, 10});
        feature.add_tag("some", "attr");
    }
    {
        vtzero::point_feature_builder<3> feature{layer_points_3d, 11 /* id */};
        feature.add_single_point({20, 20, 20});
        feature.add_tag("some", "attr");
    }

    {
        vtzero::point_feature_builder<3> feature{layer_points_3d, 12 /* id */};
        feature.add_single_point({20, 20, 30});
        feature.add_tag("some", "otherattr");
    }

    {
        vtzero::point_feature_builder<3> feature{layer_points_3d, 13 /* id */};
        feature.add_single_point({20, 20, 40});
        feature.add_tag("otherkey", "attr");
        feature.commit();
    }

    {
        vtzero::line_string_feature_builder<3> feature{layer_lines_3d, 14 /* id */};
        feature.linestring_begin(3);
        feature.add_point({10, 10, 10});
        feature.add_point({10, 20, 20});
        feature.add_point({20, 20, 30});
        feature.linestring_end();
        feature.linestring_begin(2);
        feature.add_point({11, 11, 10});
        feature.add_point({12, 13, 20});
        feature.linestring_end();
        feature.add_tag("highway", "primary");
        feature.add_tag(std::string{"maxspeed"}, vtzero::sint_value(50));
    }

    {
        vtzero::polygon_feature_builder<3> feature{layer_polygons_3d, 15 /* id */};
        feature.ring_begin(5);
        feature.add_point({0, 0, 10});
        feature.add_point({10, 0, 20});
        feature.add_point({10, 10, 30});
        feature.add_point({0, 10, 20});
        feature.add_point({0, 0, 10});
        feature.ring_end();
        feature.ring_begin(4);
        feature.add_point({3, 3, 20});
        feature.add_point({3, 5, 40});
        feature.add_point({5, 5, 30});
        feature.add_point({3, 3, 20});
        feature.ring_end();
        feature.add_tag("natural", "wood");
    }
    {
        vtzero::spline_feature_builder<3> feature{layer_splines_3d, 16 /* id */};
        // Add control points
        feature.controlpoints_begin(4);
        feature.add_point({8, 10, 10});
        feature.add_point({9, 11, 11});
        feature.add_point({11, 9, 12});
        feature.add_point({12, 10, 13});
        feature.controlpoints_end();
        // Add knots
        feature.knots_begin();
        feature.add_knot(0.0);
        feature.add_knot(0.0);
        feature.add_knot(0.0);
        feature.add_knot(1.0);
        feature.add_knot(2.0);
        feature.add_knot(2.0);
        feature.add_knot(2.0);
        feature.knots_end();
        // Add tags
        feature.tags_begin();
        feature.add_tag("natural", "curve");
        feature.tags_end();
    }

    const auto data = tile.serialize();
    const int fd = ::open("test.mvt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    assert(fd > 0);

    const auto len = ::write(fd, data.c_str(), data.size());
    assert(static_cast<size_t>(len) == data.size());
}

