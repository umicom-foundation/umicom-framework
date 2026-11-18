#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/image_canvas/CMakeLists.txt
#
# PURPOSE:
#   Compose focused checks of explicit canvas geometry and transactional RGB output.
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
    project(UmicomImageCanvasChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_canvas_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    add_library(umicom_image_canvas_subject STATIC "${_canvas_root}/src/ai/training/image_canvas.c"
        "${_canvas_root}/src/ai/training/image_tensor.c" "${_canvas_root}/src/ai/training/image_view.c")
    target_include_directories(umicom_image_canvas_subject PUBLIC "${_canvas_root}/include")
    target_compile_features(umicom_image_canvas_subject PUBLIC c_std_23)
    if(NOT MSVC)
        target_compile_options(umicom_image_canvas_subject PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    set(_canvas_library umicom_image_canvas_subject)
elseif(TARGET Umicom::ai)
    set(_canvas_library Umicom::ai)
else()
    set(_canvas_library Umicom::AiTraining)
endif()
add_executable(umicom-image-canvas-test test_image_canvas.c)
 target_link_libraries(umicom-image-canvas-test PRIVATE ${_canvas_library})
 target_compile_features(umicom-image-canvas-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-image-canvas-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(NOT WIN32)
    target_link_libraries(umicom-image-canvas-test PRIVATE m)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-image-canvas-test)
endif()
foreach(_case IN ITEMS golden identity placements arguments borrowed-color crop tensor limit capacity short-source stride overflow oversized-source origin rectangle zero)
    add_test(NAME framework.ai.training.image_canvas.${_case} COMMAND umicom-image-canvas-test ${_case})
    set_tests_properties(framework.ai.training.image_canvas.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;image;view;ownership")
endforeach()
