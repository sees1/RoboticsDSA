CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
PROJECT(astar)

# ros-specific
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(std_srvs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)

# common-packages
find_package(GTest REQUIRED)
find_package(Eigen3 REQUIRED)

enable_testing()

set(rosdep
  rclcpp
  tf2
  tf2_ros
  std_srvs
  std_msgs
  nav_msgs
  geometry_msgs
)

# DATA STRUCTS

# costmap2d
# add_library(costmap2d ${CMAKE_CURRENT_SOURCE_DIR}/src/map/costmap/costmap.cpp)
# target_include_directories(costmap2d PUBLIC include/map/costmap)
# target_link_libraries(costmap2d PUBLIC Eigen3::Eigen)
# ament_target_dependencies(costmap2d PUBLIC ${rosdep})


# # path find algo's
# add_library(astar ${CMAKE_CURRENT_SOURCE_DIR}/src/astar.cpp)
# target_include_directories(astar PUBLIC include)
# target_link_libraries(astar PUBLIC Eigen3::Eigen costmap2d) # use own map
# ament_target_dependencies(astar PUBLIC ${rosdep})

# install(
#   DIRECTORY resources
#   DESTINATION share/${PROJECT_NAME}
# )

# install(TARGETS costmap2d astar
#   RUNTIME DESTINATION lib/${PROJECT_NAME}
#   ARCHIVE DESTINATION lib
#   LIBRARY DESTINATION lib/${PROJECT_NAME}
# )

ament_package()