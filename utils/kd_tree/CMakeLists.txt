CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
PROJECT(kd_tree)

# ros-specific
find_package(ament_cmake REQUIRED)

# common-packages
find_package(Eigen3 REQUIRED)

enable_testing()

add_library(kd_tree ${CMAKE_CURRENT_SOURCE_DIR}/src/primitives.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/kd_tree_node.cpp)
target_include_directories(kd_tree PUBLIC include)
target_link_libraries(kd_tree PUBLIC Eigen3::Eigen)

# install(
#   DIRECTORY resources
#   DESTINATION share/${PROJECT_NAME}
# )

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_kd_tree
    test/test.cpp
  )

  target_link_libraries(test_kd_tree kd_tree)
endif()

install(TARGETS kd_tree
  RUNTIME DESTINATION lib/${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib/${PROJECT_NAME}
)

ament_package()