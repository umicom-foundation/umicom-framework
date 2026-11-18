# Umicom Framework
# File: tests/training_groups/CMakeLists.txt
# PURPOSE: Qualify source-group partitioning with the real shared training implementation.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
cmake_minimum_required(VERSION 3.24)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomTrainingGroupChecks LANGUAGES C)
    enable_testing()
    get_filename_component(UMICOM_GROUP_ROOT "../../" ABSOLUTE)
    add_library(umicom_training_group_core STATIC
        "${UMICOM_GROUP_ROOT}/src/ai/training/dataset_group_split.c"
        "${UMICOM_GROUP_ROOT}/src/ai/training/initialisation.c")
    target_include_directories(umicom_training_group_core PUBLIC "${UMICOM_GROUP_ROOT}/include")
    set_property(TARGET umicom_training_group_core PROPERTY C_STANDARD 23)
    set_property(TARGET umicom_training_group_core PROPERTY C_STANDARD_REQUIRED ON)
    if(NOT WIN32)
        target_link_libraries(umicom_training_group_core PUBLIC m)
    endif()
    set(_group_library umicom_training_group_core)
else()
    set(_group_library umicom_ai_training)
endif()
add_executable(umicom-training-group-split-test test_group_split.c)
target_link_libraries(umicom-training-group-split-test PRIVATE ${_group_library})
set_property(TARGET umicom-training-group-split-test PROPERTY C_STANDARD 23)
set_property(TARGET umicom-training-group-split-test PROPERTY C_STANDARD_REQUIRED ON)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-training-group-split-test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-training-group-split-test)
endif()
foreach(case isolation repeat reorder extreme-ids row-counts limit null-input null-output
             empty single-row over-limit zero-evaluation all-groups one-group short-output)
    add_test(NAME framework.ai.training.group_split.${case} COMMAND umicom-training-group-split-test ${case})
    set_tests_properties(framework.ai.training.group_split.${case} PROPERTIES TIMEOUT 15
        LABELS "framework;ai;dataset;regression")
endforeach()
