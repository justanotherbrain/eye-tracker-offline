cmake_minimum_required(VERSION 2.8)
project( eye-tracker-offline )
find_package( OpenCV REQUIRED )
add_executable( eye-tracker-offline eye-tracker-offline.cpp )
target_link_libraries( eye-tracker-offline ${OpenCV_LIBS} )


