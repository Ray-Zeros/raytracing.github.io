cmake_minimum_required(VERSION 3.10)
project(RayTracingInOneWeek)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#OPENMP
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found")
endif()


add_executable(ray_tracing_main
    main.cc
)

target_include_directories(ray_tracing_main
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

#Link OpenMP
target_link_libraries(ray_tracing_main PRIVATE OpenMP::OpenMP_CXX)
