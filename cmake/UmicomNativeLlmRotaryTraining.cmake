#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeLlmRotaryTraining.cmake
# PURPOSE:
#   Add C23 rotary forward/adjoint and numerical-derivative qualification to
#   the SAME native training library. No independent decoder or optimiser.
# PREREQUISITE:
#   Include UmicomNativeLlmRotaryResearch.cmake first so the existing RoPE
#   mathematical owner is compiled once. This module adds no replacement.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_rotary_training_root
    "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET umicom_ai_training OR NOT TARGET umicom-native-llm-rotary-probe)
    message(FATAL_ERROR
        "Include UmicomNativeLlmRotaryResearch.cmake before rotary training")
endif()

target_sources(umicom_ai_training PRIVATE
    "${_umicom_rotary_training_root}/src/ai/training/rotary_training.c")

if(NOT TARGET umicom-native-llm-rotary-gradients)
    add_executable(umicom-native-llm-rotary-gradients
        "${_umicom_rotary_training_root}/learning/native-llm/prefix-work/inspect_rotary_gradients.c")
    target_link_libraries(umicom-native-llm-rotary-gradients PRIVATE Umicom::AiTraining)
    target_compile_features(umicom-native-llm-rotary-gradients PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-native-llm-rotary-gradients PRIVATE /W4)
    else()
        target_compile_options(umicom-native-llm-rotary-gradients PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()

if(BUILD_TESTING)
    foreach(_umicom_rotary_training_case IN ITEMS gradients tensor training_rejections)
        set(_umicom_rotary_training_target
            "umicom-ai-training-rotary-${_umicom_rotary_training_case}-test")
        if(NOT TARGET "${_umicom_rotary_training_target}")
            add_executable("${_umicom_rotary_training_target}"
                "${_umicom_rotary_training_root}/tests/ai_training/test_training_rotary_${_umicom_rotary_training_case}.c")
            target_link_libraries("${_umicom_rotary_training_target}" PRIVATE
                Umicom::AiTraining)
            target_compile_features("${_umicom_rotary_training_target}" PRIVATE c_std_23)
            if(MSVC)
                target_compile_options("${_umicom_rotary_training_target}" PRIVATE /W4)
            else()
                target_compile_options("${_umicom_rotary_training_target}" PRIVATE
                    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
            endif()
            add_test(NAME "framework.ai.training.rotary_${_umicom_rotary_training_case}"
                COMMAND "${_umicom_rotary_training_target}")
            set_tests_properties(
                "framework.ai.training.rotary_${_umicom_rotary_training_case}"
                PROPERTIES LABELS "framework;ai;native-training;rotary;gradients")
        endif()
    endforeach()
    add_test(NAME framework.ai.training.rotary_gradients_demo
        COMMAND umicom-native-llm-rotary-gradients)
    set_tests_properties(framework.ai.training.rotary_gradients_demo
        PROPERTIES LABELS "framework;ai;native-training;learning;rotary;gradients")
endif()
