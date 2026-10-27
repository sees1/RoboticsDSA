CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
PROJECT(test_environment)

# ros-specific
find_package(ament_cmake REQUIRED)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()