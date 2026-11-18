#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/video_tensor/CMakeLists.txt
#
# PURPOSE:
#   Compose focused checks of temporal RGB tensor packing and frame selection without decoding.
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
    project(UmicomVideoTensorChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_video_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    add_library(umicom_video_tensor_subject STATIC "${_video_root}/src/ai/training/video_tensor.c"
        "${_video_root}/src/ai/training/image_tensor.c")
    target_include_directories(umicom_video_tensor_subject PUBLIC "${_video_root}/include")
    target_compile_features(umicom_video_tensor_subject PUBLIC c_std_23)
    if(NOT MSVC)
        target_compile_options(umicom_video_tensor_subject PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    set(_video_library umicom_video_tensor_subject)
elseif(TARGET Umicom::ai)
    set(_video_library Umicom::ai)
else()
    set(_video_library Umicom::AiTraining)
endif()
add_executable(umicom-video-tensor-test test_video_tensor.c)
 target_link_libraries(umicom-video-tensor-test PRIVATE ${_video_library})
 target_compile_features(umicom-video-tensor-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-video-tensor-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(NOT WIN32)
    target_link_libraries(umicom-video-tensor-test PRIVATE m)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-video-tensor-test)
endif()
foreach(_case IN ITEMS describe selection selection-grid selection-limits thwc tchw signed padding late-refusal maximum arguments)
    add_test(NAME framework.ai.training.video_tensor.${_case} COMMAND umicom-video-tensor-test ${_case})
    set_tests_properties(framework.ai.training.video_tensor.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;video;tensor;ownership")
endforeach()

# Output conversion preserves the complete clip when a late frame is invalid.
add_executable(umicom-video-output-test test_video_output.c)
target_link_libraries(umicom-video-output-test PRIVATE ${_video_library})
target_compile_features(umicom-video-output-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-video-output-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(NOT WIN32)
    target_link_libraries(umicom-video-output-test PRIVATE m)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-video-output-test)
endif()
foreach(_case IN ITEMS thwc tchw signed clipping padding late-nonfinite late-outside
    late-capacity late-stride late-null late-overflow limits maximum ignored-tail
    single-row round-trip value-admission arguments)
    add_test(NAME framework.ai.training.video_output.${_case} COMMAND umicom-video-output-test ${_case})
    set_tests_properties(framework.ai.training.video_output.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;video;quantization;ownership")
endforeach()
