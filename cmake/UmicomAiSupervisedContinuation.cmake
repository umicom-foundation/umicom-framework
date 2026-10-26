#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiSupervisedContinuation.cmake
# PURPOSE: Extend the existing native supervised training and projection
#          adaptation with optional checkpoint-bound exact SGD continuation.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(UMICOM_AI_CONTINUATION_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET Umicom::AiSupervisedExperiment)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiSupervisedExperiment.cmake")
endif()
if(NOT TARGET Umicom::AiDecoderProjectionAdaptation)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiDecoderProjectionAdaptation.cmake")
endif()

# The projection-snapshot methods are additive members of the EXISTING real
# decoder adapter source file; do not compile that source into a second library.
if(NOT TARGET umicom_ai_supervised_continuation)
    add_library(umicom_ai_supervised_continuation STATIC
        "${UMICOM_AI_CONTINUATION_ROOT}/src/ai/training/supervised_continuation.c"
        "${UMICOM_AI_CONTINUATION_ROOT}/src/ai/training/supervised_quality.c")
    add_library(Umicom::AiSupervisedContinuation ALIAS
        umicom_ai_supervised_continuation)
    target_include_directories(umicom_ai_supervised_continuation PUBLIC
        "${UMICOM_AI_CONTINUATION_ROOT}/include")
    target_link_libraries(umicom_ai_supervised_continuation PUBLIC
        Umicom::AiSupervisedExperiment)
    target_compile_features(umicom_ai_supervised_continuation PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_supervised_continuation PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_supervised_continuation PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

if(BUILD_TESTING)
    function(umicom_ai_continuation_test target case_name filename)
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}"
            "${UMICOM_AI_CONTINUATION_ROOT}/tests/ai_training/${filename}")
        target_link_libraries("${target}" PRIVATE Umicom::AiSupervisedContinuation)
        target_compile_features("${target}" PRIVATE c_std_23)
        if(MSVC)
            target_compile_options("${target}" PRIVATE /W4)
        else()
            target_compile_options("${target}" PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
        add_test(NAME "${case_name}" COMMAND "${target}")
        set_tests_properties("${case_name}" PROPERTIES
            LABELS "framework;ai;native-training;continuation" TIMEOUT 120)
    endfunction()

    umicom_ai_continuation_test(
        umicom-ai-projection-snapshot-test
        framework.ai.continuation.projection_snapshot
        test_training_projection_snapshot.c)
    umicom_ai_continuation_test(
        umicom-ai-projection-snapshot-rejections-test
        framework.ai.continuation.projection_snapshot_rejections
        test_training_projection_snapshot_rejections.c)
    umicom_ai_continuation_test(
        umicom-ai-supervised-continuation-test
        framework.ai.continuation.progress
        test_training_supervised_continuation.c)
    umicom_ai_continuation_test(
        umicom-ai-supervised-resume-test
        framework.ai.continuation.resume
        test_training_supervised_resume.c)
    umicom_ai_continuation_test(
        umicom-ai-supervised-resume-rejections-test
        framework.ai.continuation.resume_rejections
        test_training_supervised_resume_rejections.c)
    umicom_ai_continuation_test(
        umicom-ai-supervised-early-stop-test
        framework.ai.continuation.early_stop
        test_training_supervised_early_stop.c)
    umicom_ai_continuation_test(
        umicom-ai-supervised-continuation-rejections-test
        framework.ai.continuation.progress_rejections
        test_training_supervised_continuation_rejections.c)
    umicom_ai_continuation_test(
        umicom-ai-supervised-epoch-boundary-test
        framework.ai.continuation.epoch_boundary
        test_training_supervised_epoch_boundary_resume.c)
    umicom_ai_continuation_test(
        umicom-ai-supervised-quality-test
        framework.ai.continuation.quality
        test_training_supervised_quality.c)
    umicom_ai_continuation_test(
        umicom-ai-supervised-quality-rejections-test
        framework.ai.continuation.quality_rejections
        test_training_supervised_quality_rejections.c)
endif()
