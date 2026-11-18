#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/image_flip/CMakeLists.txt
#
# PURPOSE:
#   Compose focused checks of explicit RGB orientation, crop composition and atomic output admission.
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
    project(UmicomImageFlipChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_flip_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    add_library(umicom_image_flip_subject STATIC "${_flip_root}/src/ai/training/image_flip.c"
        "${_flip_root}/src/ai/training/image_tensor.c" "${_flip_root}/src/ai/training/image_view.c")
    target_include_directories(umicom_image_flip_subject PUBLIC "${_flip_root}/include")
    target_compile_features(umicom_image_flip_subject PUBLIC c_std_23)
    if(NOT MSVC)
        target_compile_options(umicom_image_flip_subject PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    set(_flip_library umicom_image_flip_subject)
elseif(TARGET Umicom::ai)
    set(_flip_library Umicom::ai)
else()
    set(_flip_library Umicom::AiTraining)
endif()
add_executable(umicom-image-flip-test test_image_flip.c)
 target_link_libraries(umicom-image-flip-test PRIVATE ${_flip_library})
 target_compile_features(umicom-image-flip-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-image-flip-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(NOT WIN32)
    target_link_libraries(umicom-image-flip-test PRIVATE m)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-image-flip-test)
endif()
foreach(_case IN ITEMS golden padding crop tensor refusal metadata limits involution)
    add_test(NAME framework.ai.training.image_flip.${_case} COMMAND umicom-image-flip-test ${_case})
    set_tests_properties(framework.ai.training.image_flip.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;image;view;ownership")
endforeach()
