cmake_minimum_required(VERSION 3.5)
project(test_environment)

# find dependencies
find_package(ament_cmake REQUIRED)

install(
  DIRECTORY config description launch meshes models rviz worlds maps
  DESTINATION share/${PROJECT_NAME}
)

ament_package()