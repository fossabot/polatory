// Copyright (c) 2016, GSI and The Polatory Authors.

#include <algorithm>
#include <tuple>
#include <vector>

#include <Eigen/Core>
#include <gtest/gtest.h>

#include <polatory/geometry/point3d.hpp>
#include <polatory/point_cloud/kdtree.hpp>
#include <polatory/point_cloud/random_points.hpp>

using polatory::geometry::point3d;
using polatory::geometry::points3d;
using polatory::geometry::sphere3d;
using polatory::geometry::vector3d;
using polatory::point_cloud::kdtree;
using polatory::point_cloud::random_points;

TEST(kdtree, trivial) {
  const size_t n_points = 1024;
  const double radius = 1.0;
  const point3d center(0.0, 0.0, 0.0);

  const point3d query_point = center + vector3d(radius, 0.0, 0.0);
  const size_t k = 10;
  const auto search_radius = 0.1;

  auto points = random_points(sphere3d(center, radius), n_points);

  kdtree tree(points, true);

  std::vector<size_t> indices;
  std::vector<double> distances;
  std::tie(indices, distances) = tree.knn_search(query_point, k);

  EXPECT_EQ(k, indices.size());
  EXPECT_EQ(indices.size(), distances.size());

  std::sort(indices.begin(), indices.end());
  EXPECT_EQ(indices.end(), std::unique(indices.begin(), indices.end()));

  std::tie(indices, distances) = tree.radius_search(query_point, search_radius);

  EXPECT_EQ(indices.size(), distances.size());
  for (auto distance : distances) {
    EXPECT_LE(distance, search_radius);
  }

  std::sort(indices.begin(), indices.end());
  EXPECT_EQ(indices.end(), std::unique(indices.begin(), indices.end()));
}

TEST(kdtree, zero_points) {
  const point3d query_point = point3d::Zero();
  const size_t k = 10;
  const auto search_radius = 0.1;

  points3d points;

  kdtree tree(points, true);

  std::vector<size_t> indices;
  std::vector<double> distances;
  std::tie(indices, distances) = tree.knn_search(query_point, k);

  EXPECT_EQ(0u, indices.size());
  EXPECT_EQ(0u, distances.size());

  std::tie(indices, distances) = tree.radius_search(query_point, search_radius);

  EXPECT_EQ(0u, indices.size());
  EXPECT_EQ(0u, distances.size());
}
