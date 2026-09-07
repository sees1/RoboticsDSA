CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
PROJECT(data_association)

# ros-specific
find_package(ament_cmake REQUIRED)

# common-packages
find_package(Eigen3 REQUIRED)

enable_testing()

# ALGORITHMS

# data association based on JCBB
add_library(data_association ${CMAKE_CURRENT_SOURCE_DIR}/src/math.cpp)
target_include_directories(data_association PUBLIC include)
target_link_libraries(data_association PUBLIC Eigen3::Eigen)

install(TARGETS data_association
  RUNTIME DESTINATION lib/${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib/${PROJECT_NAME}
)

ament_package()