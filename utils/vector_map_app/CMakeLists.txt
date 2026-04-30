cmake_minimum_required(VERSION 3.22)
project(vector_map_app)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(ament_index_cpp REQUIRED)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_compile_options(-std=c++2a -Wpedantic -Wall)

option(DEBUG "More output while parsing yaml" OFF)

if(DEBUG)
  add_definitions(-DDEBUG)
endif()

include_directories(include)

set(vectorMap_include
  include/vector_map_app/main_window.hpp
  include/vector_map_app/map_data.hpp
  include/vector_map_app/map_manager.hpp
  include/vector_map_app/map.hpp
  include/vector_map_app/settings_dock.hpp
  include/vector_map_app/tools_dock.hpp
)

set(vectorMap_src
  src/vector_map_app/main_window.cpp
  src/vector_map_app/map_data.cpp
  src/vector_map_app/map_manager.cpp
  src/vector_map_app/map.cpp
  src/vector_map_app/settings_dock.cpp
  src/vector_map_app/tools_dock.cpp
  src/vector_map_app/primitives/road_geometry.cpp
  src/vector_map_app/primitives/road_arc.cpp
  src/vector_map_app/primitives/road_straight.cpp
  src/vector_map_app/primitives/road_base.cpp
)

qt5_wrap_cpp(vectorMap_moc_src ${vectorMap_include})

add_library(vectorMap_lib
  ${vectorMap_moc_src}
  ${vectorMap_src}
)

set(dependencies
  rclcpp
  std_msgs
  std_srvs
  geometry_msgs
  ament_index_cpp
)

ament_target_dependencies(vectorMap_lib PUBLIC ${dependencies})
target_link_libraries(vectorMap_lib PRIVATE Qt5::Widgets)

add_executable(vectorMap_app src/utils/vector_map_app/main.cpp)
target_link_libraries(vectorMap_app PRIVATE Qt5::Widgets
                                    PUBLIC vectorMap_lib)

install(
  DIRECTORY resources
  DESTINATION share/${PROJECT_NAME}
)

install(TARGETS vectorMap_app vectorMap_lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib/${PROJECT_NAME}
)

ament_package()