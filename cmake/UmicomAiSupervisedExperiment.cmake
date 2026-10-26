#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiSupervisedExperiment.cmake
# PURPOSE:
#   Assemble the approved native supervised-data -> decoder projection SGD ->
#   held-out evidence -> portable unsigned receipt pipeline. All tensor,
#   attention, tokenisation, hashing and model code stays with its existing
#   Framework owner. Applications link one reusable Framework library.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(UMICOM_AI_SUPERVISED_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET Umicom::AiDecoderProjectionAdaptation)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiDecoderProjectionAdaptation.cmake")
endif()

if(NOT TARGET umicom_ai_supervised_experiment)
    add_library(umicom_ai_supervised_experiment STATIC
        "${UMICOM_AI_SUPERVISED_ROOT}/src/ai/training/supervised_dataset.c"
        "${UMICOM_AI_SUPERVISED_ROOT}/src/ai/training/supervised_experiment.c"
        "${UMICOM_AI_SUPERVISED_ROOT}/src/ai/training/supervised_receipt.c")
    add_library(Umicom::AiSupervisedExperiment ALIAS umicom_ai_supervised_experiment)
    target_include_directories(umicom_ai_supervised_experiment PUBLIC
        "${UMICOM_AI_SUPERVISED_ROOT}/include")
    target_link_libraries(umicom_ai_supervised_experiment PUBLIC
        Umicom::AiDecoderProjectionAdaptation)
    target_compile_features(umicom_ai_supervised_experiment PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_supervised_experiment PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_supervised_experiment PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

if(BUILD_TESTING)
    function(umicom_supervised_register_test target name source)
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}"
            "${UMICOM_AI_SUPERVISED_ROOT}/tests/ai_training/${source}")
        target_link_libraries("${target}" PRIVATE Umicom::AiSupervisedExperiment)
        target_compile_features("${target}" PRIVATE c_std_23)
        if(MSVC)
            target_compile_options("${target}" PRIVATE /W4)
        else()
            target_compile_options("${target}" PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
        add_test(NAME "${name}" COMMAND "${target}")
        set_tests_properties("${name}" PROPERTIES
            LABELS "framework;ai;supervised;native-training"
            TIMEOUT 120)
    endfunction()

    umicom_supervised_register_test(
        umicom-ai-supervised-dataset-test
        framework.ai.supervised.dataset
        test_training_supervised_dataset.c)
    umicom_supervised_register_test(
        umicom-ai-supervised-dataset-rejections-test
        framework.ai.supervised.dataset_rejections
        test_training_supervised_dataset_rejections.c)
    umicom_supervised_register_test(
        umicom-ai-supervised-experiment-test
        framework.ai.supervised.experiment
        test_training_supervised_experiment.c)
    umicom_supervised_register_test(
        umicom-ai-supervised-experiment-rejections-test
        framework.ai.supervised.experiment_rejections
        test_training_supervised_experiment_rejections.c)
    umicom_supervised_register_test(
        umicom-ai-supervised-receipt-test
        framework.ai.supervised.receipt
        test_training_supervised_receipt.c)
endif()
