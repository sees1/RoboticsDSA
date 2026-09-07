#pragma once
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include "costmap.hpp"
#include "nav_msgs/msg/path.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"

class Base {
public:
  using map_type = Costmap2D;

public:
  using Path = nav_msgs::msg::Path;
  using Pose = geometry_msgs::msg::PoseStamped;

public:
  Base(std::unique_ptr<MovePolicy> policy)
  : policy_(std::move(policy))
  {

  }

  nav_msgs::msg::Path createPlan(const geometry_msgs::msg::PoseStamped & start,
                                 const geometry_msgs::msg::PoseStamped & goal);

private:
  std::unique_ptr<MovePolicy> policy_;
};