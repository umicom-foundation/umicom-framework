#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/cmake/image_test_composition/CMakeLists.txt
#
# PURPOSE:
#   Check complete shared image-test registration with either library and both inclusion orders.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomImageTestComposition LANGUAGES C)
    enable_testing()
endif()
get_filename_component(_image_framework "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
foreach(_case IN ITEMS sdk-first training-first sdk-only training-only)
    add_test(NAME "framework.cmake.image_test_composition.${_case}"
        COMMAND "${CMAKE_COMMAND}"
            "-DCASE=${_case}"
            "-DFRAMEWORK_ROOT=${_image_framework}"
            "-DFIXTURE=${CMAKE_CURRENT_LIST_DIR}/fixture"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/fixtures/${_case}"
            "-DGENERATOR=${CMAKE_GENERATOR}"
            "-DMAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
            "-DC_COMPILER=${CMAKE_C_COMPILER}"
            "-DGENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DGENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            -P "${CMAKE_CURRENT_LIST_DIR}/check.cmake")
    set_tests_properties("framework.cmake.image_test_composition.${_case}"
        PROPERTIES TIMEOUT 120 LABELS "framework;cmake;training;composition")
endforeach()
