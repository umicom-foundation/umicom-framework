#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeLlmPositionContract.cmake
# PURPOSE:
#   Register additive C23 positional-compatibility admission and checkpoint
#   identity evidence. Neural mathematics, existing checkpoints, and historic
#   generation behaviour are not changed or duplicated by this module.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_position_contract_root
    "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET umicom_ai_training)
    message(FATAL_ERROR
        "Include UmicomNativeLlmTraining.cmake before the position contract module")
endif()

# Existing Framework SHA-256 and checkpoint codec remain the authoritative
# implementation; target_sources adds ONLY the contract orchestration file.
target_sources(umicom_ai_training PRIVATE
    "${_umicom_position_contract_root}/src/ai/training/position_contract.c")

if(NOT TARGET umicom-native-llm-position-contract)
    add_executable(umicom-native-llm-position-contract
        "${_umicom_position_contract_root}/learning/native-llm/prefix-work/inspect_position_contract.c")
    target_link_libraries(umicom-native-llm-position-contract PRIVATE Umicom::AiTraining)
    target_compile_features(umicom-native-llm-position-contract PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-native-llm-position-contract PRIVATE /W4)
    else()
        target_compile_options(umicom-native-llm-position-contract PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

if(BUILD_TESTING)
    foreach(_umicom_position_contract_case IN ITEMS roundtrip rejections benchmark)
        set(_umicom_position_contract_target
            "umicom-ai-training-position-contract-${_umicom_position_contract_case}-test")
        if(NOT TARGET "${_umicom_position_contract_target}")
            add_executable("${_umicom_position_contract_target}"
                "${_umicom_position_contract_root}/tests/ai_training/test_training_position_contract_${_umicom_position_contract_case}.c")
            target_link_libraries("${_umicom_position_contract_target}" PRIVATE
                Umicom::AiTraining)
            target_compile_features("${_umicom_position_contract_target}" PRIVATE c_std_23)
            if(MSVC)
                target_compile_options("${_umicom_position_contract_target}" PRIVATE /W4)
            else()
                target_compile_options("${_umicom_position_contract_target}" PRIVATE
                    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
            endif()
            add_test(NAME
                "framework.ai.training.position_contract_${_umicom_position_contract_case}"
                COMMAND "${_umicom_position_contract_target}")
            set_tests_properties(
                "framework.ai.training.position_contract_${_umicom_position_contract_case}"
                PROPERTIES LABELS "framework;ai;native-training;checkpoint;compatibility;position")
        endif()
    endforeach()
    add_test(NAME framework.ai.training.position_contract_demo
        COMMAND umicom-native-llm-position-contract)
    set_tests_properties(framework.ai.training.position_contract_demo
        PROPERTIES LABELS "framework;ai;native-training;learning;checkpoint;position")
endif()
