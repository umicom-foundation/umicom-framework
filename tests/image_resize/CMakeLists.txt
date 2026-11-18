#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/image_resize/CMakeLists.txt
#
# PURPOSE:
#   Compose focused checks of independent pixel-center geometry and transactional RGB output.
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
    project(UmicomImageResizeChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_resize_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    add_library(umicom_image_resize_subject STATIC "${_resize_root}/src/ai/training/image_resize.c"
        "${_resize_root}/src/ai/training/image_histogram.c"
        "${_resize_root}/src/ai/training/image_tensor.c" "${_resize_root}/src/ai/training/image_view.c")
    target_include_directories(umicom_image_resize_subject PUBLIC "${_resize_root}/include")
    target_compile_features(umicom_image_resize_subject PUBLIC c_std_23)
    if(NOT MSVC)
        target_compile_options(umicom_image_resize_subject PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    set(_resize_library umicom_image_resize_subject)
elseif(TARGET Umicom::ai)
    set(_resize_library Umicom::ai)
else()
    set(_resize_library Umicom::AiTraining)
endif()
add_executable(umicom-image-resize-test test_image_resize.c)
 target_link_libraries(umicom-image-resize-test PRIVATE ${_resize_library})
 target_compile_features(umicom-image-resize-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-image-resize-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(NOT WIN32)
    target_link_libraries(umicom-image-resize-test PRIVATE m)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-image-resize-test)
endif()
foreach(_case IN ITEMS golden identity padding crop tensor refusal metadata limits geometry information-loss)
    add_test(NAME framework.ai.training.image_resize.${_case} COMMAND umicom-image-resize-test ${_case})
    set_tests_properties(framework.ai.training.image_resize.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;image;view;ownership")
endforeach()

# The additional policy shares the same production library and strict checks;
# nearest-neighbor fixtures remain registered as a compatibility guard.
add_executable(umicom-image-bilinear-test test_bilinear.c)
target_link_libraries(umicom-image-bilinear-test PRIVATE ${_resize_library})
target_compile_features(umicom-image-bilinear-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-image-bilinear-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(NOT WIN32)
    target_link_libraries(umicom-image-bilinear-test PRIVATE m)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-image-bilinear-test)
endif()
foreach(_case IN ITEMS golden rounding identity degenerate padding crop refusal metadata limits geometry constant information-loss)
    add_test(NAME framework.ai.training.image_bilinear.${_case} COMMAND umicom-image-bilinear-test ${_case})
    set_tests_properties(framework.ai.training.image_bilinear.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;image;view;ownership")
endforeach()
