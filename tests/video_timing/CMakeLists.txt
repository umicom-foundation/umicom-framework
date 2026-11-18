#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/video_timing/CMakeLists.txt
#
# PURPOSE:
#   Build focused checks of explicit timestamp selection through its production owner.
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
    project(UmicomVideoTimingChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_timing_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    add_library(umicom_video_timing_subject STATIC "${_timing_root}/src/ai/training/video_timing.c")
    target_include_directories(umicom_video_timing_subject PUBLIC "${_timing_root}/include")
    target_compile_features(umicom_video_timing_subject PUBLIC c_std_23)
    if(NOT MSVC)
        target_compile_options(umicom_video_timing_subject PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    set(_timing_library umicom_video_timing_subject)
elseif(TARGET Umicom::ai)
    set(_timing_library Umicom::ai)
else()
    set(_timing_library Umicom::AiTraining)
endif()
add_executable(umicom-video-timing-test test_video_timing.c)
target_link_libraries(umicom-video-timing-test PRIVATE ${_timing_library})
target_compile_features(umicom-video-timing-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-video-timing-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-video-timing-test)
endif()
foreach(_case golden extremes singleton grid maximum source-null query-null output-null source-empty query-empty
    source-limit query-limit capacity duplicate-source descending-source unselected-bad-source descending-query before-first after-last)
    add_test(NAME framework.ai.training.video_timing.${_case} COMMAND umicom-video-timing-test ${_case})
    set_tests_properties(framework.ai.training.video_timing.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;video;timing;ownership")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.ai.training.video_timing.${_case} TARGETS umicom-video-timing-test)
    endif()
endforeach()
