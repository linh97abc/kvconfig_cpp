cmake_minimum_required(VERSION 3.10)

# Set the project name
project(TestProject)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/..)
find_package(KVconfig REQUIRED)

KVCONFIG_GENERATE_CPP(
    ${CMAKE_CURRENT_SOURCE_DIR}/test.json
)

# Add an executable
# add_executable(test test.cpp test2.cpp src/kv_config_base.cpp)

# target_include_directories(test PUBLIC
#     ${PROJECT_SOURCE_DIR}/src
# )

add_executable(test test.cpp test2.cpp)
target_link_libraries(test PRIVATE kvconfig)