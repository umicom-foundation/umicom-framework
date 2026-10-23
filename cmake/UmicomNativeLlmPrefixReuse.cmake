#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeLlmPrefixReuse.cmake
# PURPOSE: Additive native C23 growing-prefix study, using existing Framework
#          decoder, tensor kernel and KV-cache rather than replacing them.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_prefix_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET umicom_ai_training)
    message(FATAL_ERROR
        "Include cmake/UmicomNativeLlmTraining.cmake before UmicomNativeLlmPrefixReuse.cmake")
endif()

# This source is Framework-owned and may be shared by all applications once
# the consolidated build includes this additive module.
target_sources(umicom_ai_training PRIVATE
    "${_umicom_prefix_root}/src/ai/training/kv_prefix_reuse.c")

if(NOT TARGET umicom-native-llm-prefix-work)
    add_executable(umicom-native-llm-prefix-work
        "${_umicom_prefix_root}/learning/native-llm/prefix-work/profile_prefix_work.c")
    target_link_libraries(umicom-native-llm-prefix-work PRIVATE Umicom::AiTraining)
    target_compile_features(umicom-native-llm-prefix-work PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-native-llm-prefix-work PRIVATE /W4)
    else()
        target_compile_options(umicom-native-llm-prefix-work PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

if(BUILD_TESTING)
    foreach(_umicom_prefix_case IN ITEMS reuse rejections)
        set(_umicom_prefix_test "umicom-ai-training-kv-prefix-${_umicom_prefix_case}-test")
        if(NOT TARGET "${_umicom_prefix_test}")
            add_executable("${_umicom_prefix_test}"
                "${_umicom_prefix_root}/tests/ai_training/test_training_kv_prefix_${_umicom_prefix_case}.c")
            target_link_libraries("${_umicom_prefix_test}" PRIVATE Umicom::AiTraining)
            target_compile_features("${_umicom_prefix_test}" PRIVATE c_std_23)
            if(MSVC)
                target_compile_options("${_umicom_prefix_test}" PRIVATE /W4)
            else()
                target_compile_options("${_umicom_prefix_test}" PRIVATE
                    -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
            endif()
            add_test(NAME "framework.ai.training.kv_prefix_${_umicom_prefix_case}"
                     COMMAND "${_umicom_prefix_test}")
            set_tests_properties("framework.ai.training.kv_prefix_${_umicom_prefix_case}"
                PROPERTIES LABELS "framework;ai;native-training;kv-cache;prefix-reuse")
        endif()
    endforeach()
    add_test(NAME framework.ai.training.kv_prefix_demo
             COMMAND umicom-native-llm-prefix-work)
    set_tests_properties(framework.ai.training.kv_prefix_demo
        PROPERTIES LABELS "framework;ai;native-training;learning;kv-cache;prefix-reuse")
endif()

# Batch 71–72: the production decoder is the INDEPENDENT numerical reference
# for both final-prefix and first-eviction logits. This is additive to the
# previously qualified growing-prefix module and does not replace its code.
# The regular Framework decoder and KV-cache retain all mathematical ownership.
target_sources(umicom_ai_training PRIVATE
    "${_umicom_prefix_root}/src/ai/training/kv_position_qualification.c")

if(NOT TARGET umicom-native-llm-position-qualification)
    add_executable(umicom-native-llm-position-qualification
        "${_umicom_prefix_root}/learning/native-llm/prefix-work/inspect_position_qualification.c")
    target_link_libraries(umicom-native-llm-position-qualification PRIVATE
        Umicom::AiTraining)
    target_compile_features(umicom-native-llm-position-qualification PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-native-llm-position-qualification PRIVATE /W4)
    else()
        target_compile_options(umicom-native-llm-position-qualification PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

if(BUILD_TESTING)
    foreach(_umicom_position_case IN ITEMS qualification rejections)
        set(_umicom_position_target
            "umicom-ai-training-kv-position-${_umicom_position_case}-test")
        if(NOT TARGET "${_umicom_position_target}")
            add_executable("${_umicom_position_target}"
                "${_umicom_prefix_root}/tests/ai_training/test_training_kv_position_${_umicom_position_case}.c")
            target_link_libraries("${_umicom_position_target}" PRIVATE
                Umicom::AiTraining)
            target_compile_features("${_umicom_position_target}" PRIVATE c_std_23)
            if(MSVC)
                target_compile_options("${_umicom_position_target}" PRIVATE /W4)
            else()
                target_compile_options("${_umicom_position_target}" PRIVATE
                    -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
            endif()
            add_test(NAME "framework.ai.training.kv_position_${_umicom_position_case}"
                     COMMAND "${_umicom_position_target}")
            set_tests_properties("framework.ai.training.kv_position_${_umicom_position_case}"
                PROPERTIES LABELS "framework;ai;native-training;kv-cache;position-parity")
        endif()
    endforeach()
    add_test(NAME framework.ai.training.kv_position_demo
             COMMAND umicom-native-llm-position-qualification)
    set_tests_properties(framework.ai.training.kv_position_demo
        PROPERTIES LABELS "framework;ai;native-training;learning;kv-cache;position-parity")
endif()
