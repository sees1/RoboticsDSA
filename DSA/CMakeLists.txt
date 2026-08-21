PROJECT(robotics_dsa)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

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

# kd-tree (pivot tree)
add_library(kd_tree ${CMAKE_CURRENT_SOURCE_DIR}/src/data_struct/kd_tree/primitives.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/data_struct/kd_tree/kd_tree_node.cpp)
target_include_directories(kd_tree PUBLIC include/data_struct/kd_tree)
target_link_libraries(kd_tree PUBLIC Eigen3::Eigen)

# costmap2d
add_library(costmap2d ${CMAKE_CURRENT_SOURCE_DIR}/src/data_struct/map/costmap/costmap.cpp)
target_include_directories(costmap2d PUBLIC include/data_struct/map/costmap)
target_link_libraries(costmap2d PUBLIC Eigen3::Eigen)
ament_target_dependencies(costmap2d PUBLIC ${rosdep})

add_executable(data_struct
${PROJECT_SOURCE_DIR}/src/data_struct/test.cpp)
target_link_libraries(data_struct   PUBLIC kd_tree costmap2d
                                    PRIVATE GTest::gtest_main)

# ALGORITHMS

# data association based on JCBB
add_library(data_association ${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/data_association/math.cpp)
target_include_directories(data_association PUBLIC include/algorithms/data_association)
target_link_libraries(data_association PUBLIC Eigen3::Eigen)

# path find algo's
add_library(path_find ${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/path_find/astar.cpp)
target_include_directories(path_find PUBLIC include/algorithms/path_find)
target_link_libraries(data_association PUBLIC Eigen3::Eigen costmap2d) # use own map
ament_target_dependencies(data_association PUBLIC ${rosdep})

add_executable(algorithms
${PROJECT_SOURCE_DIR}/src/algorithms/test.cpp)
target_link_libraries(algorithms   PUBLIC data_association path_find
                                   PRIVATE GTest::gtest_main)

# install(
#   DIRECTORY resources
#   DESTINATION share/${PROJECT_NAME}
# )

install(TARGETS kd_tree costmap2d data_struct data_association path_find algorithms
  RUNTIME DESTINATION lib/${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib/${PROJECT_NAME}
)

ament_package()