# FindKVconfig.cmake - Locate the KVconfig library
# This module defines the following variables:
#  KVCONFIG_FOUND - True if KVconfig is found
#  KVCONFIG_INCLUDE_DIRS - Where to find the headers
#  KVCONFIG_LIBRARIES - The libraries to link against

# Function to generate configuration file from JSON using a Python script
set (KV_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/kv_config")
file(MAKE_DIRECTORY ${KV_GENERATED_DIR})



if(NOT DEFINED KVCONFIG_ROOT_FOLDER)
  get_filename_component(KVCONFIG_ROOT_FOLDER
                         ${CMAKE_CURRENT_LIST_DIR} ABSOLUTE)
endif()

find_path(KVCONFIG_INCLUDE_DIR
    kv_config/kv_config_base.hpp
    PATHS ${KVCONFIG_ROOT_FOLDER}/src
    NO_CMAKE_FIND_ROOT_PATH
)

mark_as_advanced(KVCONFIG_INCLUDE_DIR)

set(KVCONFIG_SRCS ${KVCONFIG_ROOT_FOLDER}/src/kv_config_base.cpp)


add_library(kvconfig STATIC EXCLUDE_FROM_ALL ${KVCONFIG_SRCS})
target_compile_features(kvconfig PUBLIC c_std_11)
target_include_directories(kvconfig PUBLIC ${KVCONFIG_INCLUDE_DIR})
target_include_directories(kvconfig PUBLIC ${KV_GENERATED_DIR})



# Function: KVCONFIG_GENERATE_CPP
# -------------------------------
# This function is responsible for generating C++ source files
# based on the provided configuration. The exact details of
# the generation process depend on the implementation within
# the function.
#
# Usage:
#   KVCONFIG_GENERATE_CPP(${json_file})
#
function(KVCONFIG_GENERATE_CPP)
    cmake_parse_arguments(KVCONFIG_GENERATE_CPP "" "" "" ${ARGN})


    set(output_file)
    set(json_file)
    foreach(FIL ${KVCONFIG_GENERATE_CPP_UNPARSED_ARGUMENTS})
        get_filename_component(FIL_WE ${FIL} NAME_WLE)
        get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
        list(APPEND output_file "${KV_GENERATED_DIR}/${FIL_WE}.kv.h")
        list(APPEND json_file ${ABS_FIL})
    endforeach()

    target_sources(kvconfig PRIVATE ${output_file})

    add_custom_command(
        OUTPUT ${output_file}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${KV_GENERATED_DIR}
        COMMAND ${CMAKE_COMMAND} -E env python ${KVCONFIG_ROOT_FOLDER}/str_config_generator.py --output_dir=${KV_GENERATED_DIR} ${json_file}
        DEPENDS ${json_file} ${KVCONFIG_ROOT_FOLDER}/str_config_generator.py
        COMMENT "Generating configuration file from ${json_file}"
    )
endfunction()


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(KVconfig DEFAULT_MSG
    KVCONFIG_INCLUDE_DIR KVCONFIG_SRCS)

