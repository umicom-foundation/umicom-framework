#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/image_view/CMakeLists.txt
#
# PURPOSE:
#   Compose focused checks of borrowed RGB crop spans and tensor composition without copying.
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
    project(UmicomImageViewChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_view_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    add_library(umicom_image_view_subject STATIC "${_view_root}/src/ai/training/image_view.c"
        "${_view_root}/src/ai/training/image_tensor.c")
    target_include_directories(umicom_image_view_subject PUBLIC "${_view_root}/include")
    target_compile_features(umicom_image_view_subject PUBLIC c_std_23)
    if(NOT MSVC)
        target_compile_options(umicom_image_view_subject PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    set(_view_library umicom_image_view_subject)
elseif(TARGET Umicom::ai)
    set(_view_library Umicom::ai)
else()
    set(_view_library Umicom::AiTraining)
endif()
add_executable(umicom-image-view-test test_image_view.c)
 target_link_libraries(umicom-image-view-test PRIVATE ${_view_library})
 target_compile_features(umicom-image-view-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-image-view-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(NOT WIN32)
    target_link_libraries(umicom-image-view-test PRIVATE m)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-image-view-test)
endif()
foreach(_case IN ITEMS identity grid padding packing inplace nested nulls rectangles metadata short limits single-row)
    add_test(NAME framework.ai.training.image_view.${_case} COMMAND umicom-image-view-test ${_case})
    set_tests_properties(framework.ai.training.image_view.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;image;view;ownership")
endforeach()
