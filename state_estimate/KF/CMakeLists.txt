CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
PROJECT(kf)

# ros-specific
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

# common-packages
find_package(Eigen3 REQUIRED)

enable_testing()

set(rosdep
  rclcpp
)

add_executable(kf ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/mesurement_models/VO_model.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/motion_models/ackermann_model.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/kf.cpp)
# add_library(kf ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_include_directories(kf PUBLIC include)
target_link_libraries(kf PUBLIC Eigen3::Eigen)

ament_target_dependencies(kf PUBLIC ${rosdep})


# if(BUILD_TESTING)
#   find_package(ament_cmake_gtest REQUIRED)

#   ament_add_gtest(test_kd_tree
#     test/test.cpp
#   )

#   target_link_libraries(test_kd_tree kd_tree)
# endif()

install(TARGETS kf
  RUNTIME DESTINATION lib/${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib/${PROJECT_NAME}
)

ament_package()