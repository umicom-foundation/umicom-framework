#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/training_metrics/CMakeLists.txt
#
# PURPOSE:
#   Compose focused checks of the production token-rank counter without model execution.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
get_filename_component(_metrics_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomPredictionMetricsChecks LANGUAGES C)
    include(CTest)
    add_library(umicom_metrics_subject STATIC "${_metrics_root}/src/ai/training/prediction_metrics.c")
    target_include_directories(umicom_metrics_subject PUBLIC "${_metrics_root}/include")
    target_compile_features(umicom_metrics_subject PUBLIC c_std_23)
    if(NOT MSVC)
        target_compile_options(umicom_metrics_subject PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    if(NOT WIN32)
        target_link_libraries(umicom_metrics_subject PUBLIC m)
    endif()
    set(_metrics_library umicom_metrics_subject)
elseif(TARGET Umicom::ai)
    # The full Framework graph verifies the public SDK component.
    set(_metrics_library Umicom::ai)
else()
    set(_metrics_library Umicom::AiTraining)
endif()
add_executable(umicom-prediction-metrics-test test_prediction_metrics.c)
target_link_libraries(umicom-prediction-metrics-test PRIVATE ${_metrics_library})
target_compile_features(umicom-prediction-metrics-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-prediction-metrics-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-prediction-metrics-test)
endif()
foreach(_case IN ITEMS gold selection empty-selection ties reference maximum arguments capacity selection-refusal nonfinite target-refusal)
    add_test(NAME framework.ai.training.prediction_metrics.${_case} COMMAND umicom-prediction-metrics-test ${_case})
    set_tests_properties(framework.ai.training.prediction_metrics.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;evaluation;metrics")
endforeach()

# Loss reduction belongs to the AI SDK and graph execution belongs to the
# optional trainer. Use the trainer when the parent provides it. Otherwise
# link the existing small production tape fixture to the test only, including
# in the SDK-only Applications build. The parity case still runs in both.
if(TARGET Umicom::AiTraining)
    set(_metrics_tape_library Umicom::AiTraining)
else()
    add_library(umicom_metrics_tape_subject STATIC
        "${_metrics_root}/src/ai/training/tape.c"
        "${_metrics_root}/src/ai/training/matrix_kernel.c")
    target_include_directories(umicom_metrics_tape_subject PUBLIC "${_metrics_root}/include")
    target_compile_features(umicom_metrics_tape_subject PUBLIC c_std_23)
    if(NOT MSVC)
        target_compile_options(umicom_metrics_tape_subject PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    if(NOT WIN32)
        target_link_libraries(umicom_metrics_tape_subject PUBLIC m)
    endif()
    set(_metrics_tape_library umicom_metrics_tape_subject)
endif()
# The earlier integrated rule is retained for review. It inferred graph
# ownership from the metrics archive, which omits tape in SDK-only builds.
#[=[
else()
    set(_metrics_tape_library ${_metrics_library})
endif()
]=]
add_executable(umicom-prediction-loss-test test_prediction_loss.c)
target_link_libraries(umicom-prediction-loss-test PRIVATE ${_metrics_library} ${_metrics_tape_library})
target_compile_features(umicom-prediction-loss-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-prediction-loss-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(NOT WIN32)
    target_link_libraries(umicom-prediction-loss-test PRIVATE m)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-prediction-loss-test)
endif()
foreach(_case IN ITEMS uniform probability large-offset extremes offset-invariance
    selection empty-selection aggregate nonfinite refusals maximum ownership tape-parity)
    add_test(NAME framework.ai.training.prediction_loss.${_case} COMMAND umicom-prediction-loss-test ${_case})
    set_tests_properties(framework.ai.training.prediction_loss.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;evaluation;loss")
endforeach()

# Evaluate summaries only: no model or optimiser is created.
add_executable(umicom-prediction-summary-test test_prediction_summary.c)
target_link_libraries(umicom-prediction-summary-test PRIVATE ${_metrics_library})
target_compile_features(umicom-prediction-summary-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-prediction-summary-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(NOT WIN32)
    target_link_libraries(umicom-prediction-summary-test PRIVATE m)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-prediction-summary-test)
endif()
foreach(_case IN ITEMS merge identity aliases streamed empty mean perplexity
    malformed-count malformed-empty negative nan infinity overflow-count overflow-sum overflow-exp nulls)
    add_test(NAME framework.ai.training.prediction_summary.${_case} COMMAND umicom-prediction-summary-test ${_case})
    set_tests_properties(framework.ai.training.prediction_summary.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;evaluation;loss;aggregation")
endforeach()
