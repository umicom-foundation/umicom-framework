#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/training_decoder_mask/CMakeLists.txt
#
# PURPOSE:
#   Build focused selected-decoder qualification from the production training sources.
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
    project(UmicomDecoderMaskChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_decoder_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    add_library(umicom_decoder_mask_subject STATIC)
    # Earlier standalone list omitted capacity_plan required by the retained wide-decoder test:
    # tape matrix_kernel initialisation transformer decoder optimizer gradient_clipping checkpoint dataset_split
    # Compile the same capacity service used by the production Framework target.
    foreach(_source IN ITEMS tape matrix_kernel initialisation transformer decoder optimizer gradient_clipping checkpoint optimizer_checkpoint dataset_split capacity_plan)
        target_sources(umicom_decoder_mask_subject PRIVATE "${_decoder_root}/src/ai/training/${_source}.c")
    endforeach()
    target_include_directories(umicom_decoder_mask_subject PUBLIC "${_decoder_root}/include")
    target_compile_features(umicom_decoder_mask_subject PUBLIC c_std_23)
    if(NOT WIN32)
        target_link_libraries(umicom_decoder_mask_subject PUBLIC m)
    endif()
    if(NOT MSVC)
        target_compile_options(umicom_decoder_mask_subject PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    set(_decoder_library umicom_decoder_mask_subject)
else()
    set(_decoder_library Umicom::AiTraining)
endif()
add_executable(umicom-decoder-mask-test test_decoder_mask.c)
target_link_libraries(umicom-decoder-mask-test PRIVATE ${_decoder_library})
target_compile_features(umicom-decoder-mask-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-decoder-mask-test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-decoder-mask-test)
endif()
foreach(_case IN ITEMS reference full-mask-parity excluded-targets rejections budget-rollback repeated-steps causal-context checkpoint-resume batch-weighting step-overflow)
    add_test(NAME framework.ai.training.decoder_mask.${_case} COMMAND umicom-decoder-mask-test ${_case})
    set_tests_properties(framework.ai.training.decoder_mask.${_case} PROPERTIES TIMEOUT 60 LABELS "framework;ai;training;decoder;masked-loss")
endforeach()

# Existing whole-decoder tests protect compatibility beyond the new objective.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    foreach(_case IN ITEMS decoder_contract decoder_train decoder_memory checkpoint decoder_adamw decoder_clipped gradient_accumulation accumulated_limits wide_decoder capacity_plan)
        add_executable(decoder-mask-regression-${_case} "../ai_training/test_training_${_case}.c")
        target_link_libraries(decoder-mask-regression-${_case} PRIVATE ${_decoder_library})
        add_test(NAME framework.ai.training.decoder_mask_regression.${_case} COMMAND decoder-mask-regression-${_case})
        set_tests_properties(framework.ai.training.decoder_mask_regression.${_case} PROPERTIES TIMEOUT 120)
    endforeach()
endif()

add_executable(umicom-decoder-padding-test test_decoder_padding.c)
target_link_libraries(umicom-decoder-padding-test PRIVATE ${_decoder_library})
target_compile_features(umicom-decoder-padding-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-decoder-padding-test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-decoder-padding-test)
endif()
foreach(_case IN ITEMS reference padding-invariance training-invariance legacy-parity empty isolation rejections budget repeat resume step-overflow)
    add_test(NAME framework.ai.training.decoder_padding.${_case} COMMAND umicom-decoder-padding-test ${_case})
    set_tests_properties(framework.ai.training.decoder_padding.${_case} PROPERTIES TIMEOUT 60 LABELS "framework;ai;training;decoder;padding")
endforeach()
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    foreach(_case IN ITEMS transformer transformer_gradients transformer_training)
        add_executable(decoder-padding-regression-${_case} "../ai_training/test_training_${_case}.c")
        target_link_libraries(decoder-padding-regression-${_case} PRIVATE ${_decoder_library})
        add_test(NAME framework.ai.training.decoder_padding_regression.${_case} COMMAND decoder-padding-regression-${_case})
        set_tests_properties(framework.ai.training.decoder_padding_regression.${_case} PROPERTIES TIMEOUT 120)
    endforeach()
endif()

# Stateful selection checks compare complete weights-and-moments checkpoints.
add_executable(umicom-decoder-adamw-selection-test test_decoder_adamw_selection.c)
target_link_libraries(umicom-decoder-adamw-selection-test PRIVATE ${_decoder_library})
target_compile_features(umicom-decoder-adamw-selection-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-decoder-adamw-selection-test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-decoder-adamw-selection-test)
endif()
foreach(_case IN ITEMS legacy-parity selection-parity reference target-invariance padding-invariance rejections binding budget repeat resume update-failure step-overflow)
    add_test(NAME framework.ai.training.decoder_adamw_selection.${_case}
        COMMAND umicom-decoder-adamw-selection-test ${_case})
    set_tests_properties(framework.ai.training.decoder_adamw_selection.${_case} PROPERTIES
        TIMEOUT 90 LABELS "framework;ai;training;decoder;adamw;selection")
endforeach()

# Effective batches are reduced by selected-row counts, then committed once.
add_executable(umicom-decoder-selected-accumulation-test test_decoder_selected_accumulation.c)
target_link_libraries(umicom-decoder-selected-accumulation-test PRIVATE ${_decoder_library})
target_compile_features(umicom-decoder-selected-accumulation-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-decoder-selected-accumulation-test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-decoder-selected-accumulation-test)
endif()
foreach(_case IN ITEMS whole-parity legacy-parity selection-parity weighted-loss large-reference invariance rejections memory budget resume binding update-failure step-overflow)
    add_test(NAME framework.ai.training.selected_accumulation.${_case}
        COMMAND umicom-decoder-selected-accumulation-test ${_case})
    set_tests_properties(framework.ai.training.selected_accumulation.${_case} PROPERTIES
        TIMEOUT 90 LABELS "framework;ai;training;decoder;adamw;accumulation;selection")
endforeach()

add_executable(umicom-accumulation-plan-test test_accumulation_plan.c)
target_link_libraries(umicom-accumulation-plan-test PRIVATE ${_decoder_library})
target_compile_features(umicom-accumulation-plan-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-accumulation-plan-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-accumulation-plan-test)
endif()
foreach(_case IN ITEMS values partition limits refusal graph-boundary)
    add_test(NAME framework.ai.training.accumulation_plan.${_case}
        COMMAND umicom-accumulation-plan-test ${_case})
    set_tests_properties(framework.ai.training.accumulation_plan.${_case} PROPERTIES
        TIMEOUT 30 LABELS "framework;ai;training;capacity;accumulation")
endforeach()
