cmake_minimum_required(VERSION 3.6.0)
project(RayTracing VERSION 0.1.0)

include(CTest)
enable_testing()


include_directories(core stb cameras materials accelerators textures ext shapes) 
aux_source_directory(. SRC_FILES)
add_executable(RayTracing ${SRC_FILES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
