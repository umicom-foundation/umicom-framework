#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/training_loss/CMakeLists.txt
#
# PURPOSE:
#   Check native categorical loss and gradients in isolation or in the normal training graph.
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
    project(UmicomTrainingLossChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_loss_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    add_library(umicom_loss_subject STATIC "${_loss_root}/src/ai/training/tape.c"
        "${_loss_root}/src/ai/training/matrix_kernel.c")
    target_include_directories(umicom_loss_subject PUBLIC "${_loss_root}/include")
    target_compile_features(umicom_loss_subject PUBLIC c_std_23)
    if(NOT WIN32)
        target_link_libraries(umicom_loss_subject PUBLIC m)
    endif()
    if(NOT MSVC)
        target_compile_options(umicom_loss_subject PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    set(_loss_library umicom_loss_subject)
else()
    set(_loss_library Umicom::AiTraining)
endif()
foreach(_kind IN ITEMS masked_loss loss_allocation masked_squared_error masked_attention weighted_squared_error)
    add_executable(umicom-${_kind}-test "test_${_kind}.c")
    target_link_libraries(umicom-${_kind}-test PRIVATE ${_loss_library})
    target_compile_features(umicom-${_kind}-test PRIVATE c_std_23)
    if(NOT MSVC)
        target_compile_options(umicom-${_kind}-test PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-${_kind}-test)
    endif()
endforeach()
foreach(_case IN ITEMS large-offset reference uniform-gradient finite-difference all-row-parity
    owned-selection budget rejections arguments shared-gradient)
    add_test(NAME framework.ai.training.masked_loss.${_case} COMMAND umicom-masked_loss-test ${_case})
    set_tests_properties(framework.ai.training.masked_loss.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;ai;native-training;loss;ownership")
endforeach()
add_test(NAME framework.ai.training.loss_allocation COMMAND umicom-loss_allocation-test)
set_tests_properties(framework.ai.training.loss_allocation PROPERTIES TIMEOUT 30 LABELS "framework;ai;native-training;allocation")

# Reuse existing tape regressions without another production algorithm. Normal
 # Framework registration already owns these cases, so add them only standalone.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    foreach(_case IN ITEMS contract gradients numerical_gradients attention attention_gradients
        multihead_gradients layernorm_gradients memory_budget memory_auxiliary wide_tape matrix_tape)
        add_executable(loss-regression-${_case} "../ai_training/test_training_${_case}.c")
        target_link_libraries(loss-regression-${_case} PRIVATE ${_loss_library})
        add_test(NAME framework.ai.training.loss_regression.${_case} COMMAND loss-regression-${_case})
        set_tests_properties(framework.ai.training.loss_regression.${_case} PROPERTIES TIMEOUT 60)
    endforeach()
endif()

# The element mask is independent of categorical row selection; exercise both.
foreach(_case IN ITEMS reference finite-difference all-selected excluded-extremes full-capacity
    owned-selection budget rejections arguments shape numeric-refusal shared-gradient same-input rollback node-capacity)
    add_test(NAME framework.ai.training.masked_squared_error.${_case} COMMAND umicom-masked_squared_error-test ${_case})
    set_tests_properties(framework.ai.training.masked_squared_error.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;ai;native-training;loss;ownership")
endforeach()

# Attention row selection and objective selection are independent contracts.
foreach(_case IN ITEMS reference finite-difference alias-gradient all-selected all-excluded isolation excluded-extremes ownership budget rejections shared-gradient rollback capacity)
    add_test(NAME framework.ai.training.masked_attention.${_case} COMMAND umicom-masked_attention-test ${_case})
    set_tests_properties(framework.ai.training.masked_attention.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;ai;native-training;attention;ownership")
endforeach()

# Continuous weights normalize by their sum, independently of binary masks.
foreach(_case IN ITEMS reference finite-difference rescaling full-capacity excluded-extremes
    binary-parity uniform-parity ownership budget refusal arguments shape numeric-refusal
    same-input shared-gradient repeat node-capacity refused-after-backward)
    add_test(NAME framework.ai.training.weighted_squared_error.${_case}
        COMMAND umicom-weighted_squared_error-test ${_case})
    set_tests_properties(framework.ai.training.weighted_squared_error.${_case} PROPERTIES
        TIMEOUT 30 LABELS "framework;ai;native-training;loss;weights;ownership")
endforeach()
