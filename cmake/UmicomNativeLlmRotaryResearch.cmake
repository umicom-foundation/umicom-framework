#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeLlmRotaryResearch.cmake
# PURPOSE:
#   Add an isolated, opt-in C23 RoPE/Rolling-KV mathematical laboratory to the
#   existing Framework-owned native AI training target. This composes rather
#   than replaces prior model, absolute-position, checkpoint or sampler code.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_rotary_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET umicom_ai_training)
    message(FATAL_ERROR
        "Include UmicomNativeLlmTraining.cmake before rotary research")
endif()

target_sources(umicom_ai_training PRIVATE
    "${_umicom_rotary_root}/src/ai/training/rotary_position.c"
    "${_umicom_rotary_root}/src/ai/training/rotary_window.c")

if(NOT TARGET umicom-native-llm-rotary-probe)
    add_executable(umicom-native-llm-rotary-probe
        "${_umicom_rotary_root}/learning/native-llm/prefix-work/inspect_rotary_window.c")
    target_link_libraries(umicom-native-llm-rotary-probe PRIVATE Umicom::AiTraining)
    target_compile_features(umicom-native-llm-rotary-probe PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-native-llm-rotary-probe PRIVATE /W4)
    else()
        target_compile_options(umicom-native-llm-rotary-probe PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

if(BUILD_TESTING)
    foreach(_umicom_rotary_case IN ITEMS position window rejections)
        set(_umicom_rotary_target
            "umicom-ai-training-rotary-${_umicom_rotary_case}-test")
        if(NOT TARGET "${_umicom_rotary_target}")
            add_executable("${_umicom_rotary_target}"
                "${_umicom_rotary_root}/tests/ai_training/test_training_rotary_${_umicom_rotary_case}.c")
            target_link_libraries("${_umicom_rotary_target}" PRIVATE Umicom::AiTraining)
            target_compile_features("${_umicom_rotary_target}" PRIVATE c_std_23)
            if(MSVC)
                target_compile_options("${_umicom_rotary_target}" PRIVATE /W4)
            else()
                target_compile_options("${_umicom_rotary_target}" PRIVATE
                    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
            endif()
            add_test(NAME "framework.ai.training.rotary_${_umicom_rotary_case}"
                COMMAND "${_umicom_rotary_target}")
            set_tests_properties(
                "framework.ai.training.rotary_${_umicom_rotary_case}"
                PROPERTIES LABELS "framework;ai;native-training;rotary;single-block")
        endif()
    endforeach()
    add_test(NAME framework.ai.training.rotary_demo
             COMMAND umicom-native-llm-rotary-probe)
    set_tests_properties(framework.ai.training.rotary_demo PROPERTIES
        LABELS "framework;ai;native-training;rotary;learning")
endif()
