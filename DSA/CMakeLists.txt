PROJECT(DSA)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# ros-specific
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(ament_index_cpp REQUIRED)

# common-packages
find_package(GTest REQUIRED)
find_package(Eigen3 REQUIRED)

enable_testing()

# DATA STRUCTS

# kd-tree (pivot tree)
add_library(kd_tree ${CMAKE_CURRENT_SOURCE_DIR}/src/data_struct/kd_tree/primitives.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/data_struct/kd_tree/kd_tree_node.cpp)
target_include_directories(kd_tree PUBLIC include/data_struct/kd_tree)
target_link_libraries(kd_tree PUBLIC Eigen3::Eigen)

add_executable(data_struct
${PROJECT_SOURCE_DIR}/src/data_struct/test.cpp)
target_link_libraries(data_struct   PUBLIC kd_tree
                                    PRIVATE GTest::gtest_main)

# ALGORITHMS

add_library(data_association ${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/data_association/math.cpp)
target_include_directories(data_association PUBLIC include/data_association)
target_link_libraries(data_association PUBLIC Eigen3::Eigen)

add_executable(algorithms
${PROJECT_SOURCE_DIR}/src/algorithms/test.cpp)
target_link_libraries(algorithms   PUBLIC data_association
                                   PRIVATE GTest::gtest_main)