#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/cmake/image_test_composition/fixture/CMakeLists.txt
#
# PURPOSE:
#   Verify shared test targets, chosen dependency and complete case registration in a small consumer graph.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
project(UmicomImageTestConsumers LANGUAGES C)
enable_testing()
if(NOT CASE MATCHES "^(sdk-first|training-first|sdk-only|training-only)$")
    message(FATAL_ERROR "Unknown image test composition case: ${CASE}")
endif()
# These interface targets model library availability only. This fixture tests
# CMake composition; the standalone image suite separately compiles real C.
function(add_consumer kind)
    if(kind STREQUAL "sdk")
        add_library(image_sdk INTERFACE)
        add_library(Umicom::ai ALIAS image_sdk)
    else()
        add_library(image_training INTERFACE)
        add_library(Umicom::AiTraining ALIAS image_training)
    endif()
endfunction()
function(umicom_register_validation_target target)
    set_property(GLOBAL APPEND PROPERTY IMAGE_REGISTERED_TARGETS "${target}")
endfunction()
if(CASE MATCHES "^sdk")
    add_consumer(sdk)
    set(_expected_library Umicom::ai)
else()
    add_consumer(training)
    set(_expected_library Umicom::AiTraining)
endif()
add_subdirectory("${FRAMEWORK_ROOT}/tests/image_tensor" first)
if(CASE STREQUAL "sdk-first")
    add_consumer(training)
elseif(CASE STREQUAL "training-first")
    add_consumer(sdk)
endif()
if(CASE MATCHES "-first$")
    # Match two Framework composition modules with different binary folders.
    add_subdirectory("${FRAMEWORK_ROOT}/tests/image_tensor" second)
    get_property(_second_tests DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/second" PROPERTY TESTS)
    if(_second_tests)
        message(FATAL_ERROR "The second consumer registered duplicate cases: ${_second_tests}")
    endif()
endif()
set(_targets umicom-image-tensor-test umicom-image-standardization-test umicom-image-quantization-test)
foreach(_target IN LISTS _targets)
    if(NOT TARGET "${_target}")
        message(FATAL_ERROR "Missing image test target: ${_target}")
    endif()
    get_target_property(_library "${_target}" LINK_LIBRARIES)
    list(FIND _library "${_expected_library}" _found)
    if(_found EQUAL -1)
        message(FATAL_ERROR "${_target} lost its first available library: ${_library}")
    endif()
endforeach()
get_property(_registered GLOBAL PROPERTY IMAGE_REGISTERED_TARGETS)
list(SORT _targets)
list(SORT _registered)
if(NOT "${_targets}" STREQUAL "${_registered}")
    message(FATAL_ERROR "Validation registration is incomplete or duplicated: ${_registered}")
endif()
set(_tensor_cases hwc chw signed padding single-row reference maximum shape-refusal span-refusal arguments)
set(_standardization_cases golden chw identity all-values-reference maximum invalid-scale invalid-mean
    endpoint-overflow extreme-finite shape-refusal span-refusal arguments)
set(_quantization_cases unit signed chw halfway clipping outside nonfinite padding single-row
    input-tail all-bytes maximum shape-refusal span-refusal arguments)
set(_expected_tests)
foreach(_kind IN ITEMS tensor standardization quantization)
    foreach(_case IN LISTS _${_kind}_cases)
        list(APPEND _expected_tests "framework.ai.training.image_${_kind}.${_case}")
    endforeach()
endforeach()
get_property(_actual_tests DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/first" PROPERTY TESTS)
list(SORT _expected_tests)
list(SORT _actual_tests)
if(NOT "${_expected_tests}" STREQUAL "${_actual_tests}")
    message(FATAL_ERROR "Image test coverage changed: expected ${_expected_tests}, got ${_actual_tests}")
endif()
message(STATUS "Complete image test registration verified: ${CASE}")
