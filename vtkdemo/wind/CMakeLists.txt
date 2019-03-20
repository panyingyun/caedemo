
cmake_minimum_required(VERSION 2.8)
 
PROJECT(wind)
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

aux_source_directory(. DIR_SRCS)
 
add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${DIR_SRCS} )
 
target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES})
