#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/cmake/training_source_inputs/CMakeLists.txt
#
# PURPOSE:
#   Register file-admission regressions without compiling Framework applications.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomTrainingInputChecks LANGUAGES NONE)
    enable_testing()
endif()
get_filename_component(_training_root "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
foreach(_case IN ITEMS valid missing-pairs-header missing-summary-header missing-file-header missing-partition-header missing-selection-header missing-denoising-header missing-pairs-source missing-writer-source missing-summary-source missing-file-source missing-partition-source missing-selection-source missing-denoising-source missing-normal-source missing-schedule-source missing-order-header missing-order-source missing-metrics-header missing-metrics-source missing-image-header missing-image-source missing-video-header missing-video-source missing-view-header missing-view-source missing-byte-pair-header missing-byte-pair-source missing-overlap-header missing-overlap-source missing-flip-header missing-flip-source missing-histogram-header missing-histogram-source missing-resize-header missing-resize-source missing-canvas-header missing-canvas-source missing-shape-source missing-context-header missing-context-source missing-timing-header missing-timing-source missing-stream-header missing-stream-source missing-review-header missing-review-source missing-moments-header missing-moments-source missing-categorical-kernel empty-header directory-header multiple-missing missing-root)
    add_test(NAME "framework.training_sources.${_case}"
        COMMAND "${CMAKE_COMMAND}" "-DCASE=${_case}"
        "-DMODULE=${_training_root}/cmake/UmicomTrainingSourceInputs.cmake"
        "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/fixtures/${_case}"
        -P "${CMAKE_CURRENT_LIST_DIR}/check.cmake")
    set_tests_properties("framework.training_sources.${_case}" PROPERTIES
        TIMEOUT 20 LABELS "framework;cmake;source-admission")
endforeach()
