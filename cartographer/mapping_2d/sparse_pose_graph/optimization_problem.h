/*
 * Copyright 2016 The Cartographer Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CARTOGRAPHER_MAPPING_2D_SPARSE_POSE_GRAPH_OPTIMIZATION_PROBLEM_H_
#define CARTOGRAPHER_MAPPING_2D_SPARSE_POSE_GRAPH_OPTIMIZATION_PROBLEM_H_

#include <array>
#include <map>
#include <vector>

#include "Eigen/Core"
#include "Eigen/Geometry"
#include "cartographer/common/port.h"
#include "cartographer/common/time.h"
#include "cartographer/mapping/mapping_sparse_pose_graph.h"
#include "cartographer/mapping/sparse_pose_graph/proto/optimization_problem_options.pb.h"
#include "cartographer/mapping_2d/submaps.h"
#include "cartographer/mapping_2d/imu_integration.h"

namespace cartographer {
namespace mapping_2d {
namespace sparse_pose_graph {

struct NodeData {
  // TODO(whess): Keep nodes per trajectory instead.
  const mapping::Submaps* trajectory;
  common::Time time;
  transform::Rigid2d initial_point_cloud_pose;
  transform::Rigid2d point_cloud_pose;
};

// Implements the SPA loop closure method.
class OptimizationProblem {
 public:
  using Constraint = mapping::SparsePoseGraph::Constraint;

  explicit OptimizationProblem(
      const mapping::sparse_pose_graph::proto::OptimizationProblemOptions&
          options);
  ~OptimizationProblem();

  OptimizationProblem(const OptimizationProblem&) = delete;
  OptimizationProblem& operator=(const OptimizationProblem&) = delete;

  void AddImuData(const mapping::Submaps* trajectory, common::Time time,
                  const Eigen::Vector3d& linear_acceleration,
                  const Eigen::Vector3d& angular_velocity);
  void AddTrajectoryNode(const mapping::Submaps* trajectory, common::Time time,
                         const transform::Rigid2d& initial_point_cloud_pose,
                         const transform::Rigid2d& point_cloud_pose);

  void SetMaxNumIterations(int32 max_num_iterations);

  // Computes the optimized poses.
  void Solve(const std::vector<Constraint>& constraints,
             std::vector<transform::Rigid2d>* submap_transforms);

  const std::vector<NodeData>& node_data() const;

 private:
  mapping::sparse_pose_graph::proto::OptimizationProblemOptions options_;
  std::map<const mapping::Submaps*, std::deque<mapping_2d::ImuData>> imu_data_;
  std::vector<NodeData> node_data_;
};

}  // namespace sparse_pose_graph
}  // namespace mapping_2d
}  // namespace cartographer

#endif  // CARTOGRAPHER_MAPPING_2D_SPARSE_POSE_GRAPH_OPTIMIZATION_PROBLEM_H_
