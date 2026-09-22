cmake_minimum_required(VERSION 3.20)

project(RayTracer655 LANGUAGES CXX)

add_executable(RayTracer655 Raytracer.cpp)
target_compile_features(RayTracer655 PRIVATE cxx_std_17)

target_include_directories(RayTracer655 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(RayTracer655 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)
