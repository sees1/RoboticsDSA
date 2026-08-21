#pragma once
#include <vector>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <cmath>

#include <Eigen/Dense>

#include <rclcpp/rclcpp.hpp>
#include <tf2_ros/transform_broadcaster.hpp>

#include "nav_msgs/msg/occupancy_grid.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"


class Costmap2D : public rclcpp::Node {
public:
  using MPoint = Eigen::Vector2i; // map point
  using WPoint = Eigen::Vector2d; // world point
  using RPoint = geometry_msgs::msg::PoseStamped; // ros point
  using Transform = geometry_msgs::msg::TransformStamped;

public:
  explicit Costmap2D(size_t w, size_t h, double resolution)
  : Node("costmap2d"),
    width_(w),
    height_(h),
    data_(w * h, 0)
  {
    tf_broad_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
    tf_timer_ = create_wall_timer(std::chrono::milliseconds(100), [this]()
    {
      Transform t;

      t.header.stamp = this->now();
      t.header.frame_id = "world";
      t.child_frame_id = "map";

      t.transform.translation.x = 0.0;
      t.transform.translation.y = 0.0;
      t.transform.translation.z = 0.0;

      t.transform.rotation.x = 0.0;
      t.transform.rotation.y = 0.0;
      t.transform.rotation.z = 0.0;
      t.transform.rotation.w = 1.0;

      tf_broad_->sendTransform(t);
    });
  }

  void setCost(const RPoint& point, uint8_t new_cost) { data_[getIndex(worldToMap(utils::flatenizePoint(point)))] = new_cost; }
  uint8_t getCost(const RPoint& point) const { return data_[getIndex(worldToMap(utils::flatenizePoint(point)))]; }

  bool inBounds(const RPoint& point) const { return inBounds(utils::flatenizePoint(point)); }
  bool inBounds(const WPoint& point) const
  {
    if (point(0) < 0.0 || point(1) < 0.0 || point(0) >= width_ * resolution_ || point(1) >= height_ * resolution_)
      return false;
    return true;
  }

private:
  MPoint worldToMap(const WPoint& point) const
  {
    if (!inBounds(point))
      throw std::runtime_error("Costmap2D::worldToMap: try to get tile out of map bound!");

    // point coord already at map frame
    return {static_cast<uint8_t>(point(0) / resolution_), static_cast<uint8_t>(point(1) / resolution_)};
  }
  WPoint mapToWorld(const MPoint& point) const { return {(point(0) + 0.5) * resolution_, (point(1) + 0.5) * resolution_}; }

  size_t getIndex(const MPoint& point) const { return point(1) * width_ + point(0); }
private:
  int width_;
  int height_;
  double resolution_;
  std::vector<uint8_t> data_;

  std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broad_;
  rclcpp::TimerBase::SharedPtr tf_timer_;
};