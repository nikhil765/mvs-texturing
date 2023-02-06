project(EstimateWidth)
cmake_minimum_required(VERSION 3.0)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(parse_text main.cpp)
target_link_libraries(parse_text ${OpenCV_LIBS})