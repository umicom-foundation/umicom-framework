#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeLlmRotaryTape.cmake
# PURPOSE: Opt-in C23 automatic-differentiation bridge for RoPE via existing
#          Framework tape operators. No replacement tape or checkpoint codec.
# PREREQUISITES: UmicomNativeLlmTraining.cmake and RotaryTraining.cmake.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_rotary_tape_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET umicom_ai_training OR NOT TARGET umicom-native-llm-rotary-gradients)
    message(FATAL_ERROR "Include UmicomNativeLlmTraining.cmake and UmicomNativeLlmRotaryTraining.cmake first")
endif()

target_sources(umicom_ai_training PRIVATE
    "${_umicom_rotary_tape_root}/src/ai/training/rotary_tape.c")

if(NOT TARGET umicom-native-llm-rotary-tape)
    add_executable(umicom-native-llm-rotary-tape
        "${_umicom_rotary_tape_root}/learning/native-llm/prefix-work/inspect_rotary_tape.c")
    target_link_libraries(umicom-native-llm-rotary-tape PRIVATE Umicom::AiTraining)
    target_compile_features(umicom-native-llm-rotary-tape PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-native-llm-rotary-tape PRIVATE /W4)
    else()
        target_compile_options(umicom-native-llm-rotary-tape PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()

if(BUILD_TESTING)
    foreach(_umicom_tape_case IN ITEMS forward rejections)
        set(_umicom_tape_target "umicom-ai-training-rotary-tape-${_umicom_tape_case}-test")
        if(NOT TARGET "${_umicom_tape_target}")
            if(_umicom_tape_case STREQUAL "forward")
                set(_umicom_tape_source "test_training_rotary_tape.c")
            else()
                set(_umicom_tape_source "test_training_rotary_tape_rejections.c")
            endif()
            add_executable("${_umicom_tape_target}"
                "${_umicom_rotary_tape_root}/tests/ai_training/${_umicom_tape_source}")
            target_link_libraries("${_umicom_tape_target}" PRIVATE Umicom::AiTraining)
            target_compile_features("${_umicom_tape_target}" PRIVATE c_std_23)
            if(MSVC)
                target_compile_options("${_umicom_tape_target}" PRIVATE /W4)
            else()
                target_compile_options("${_umicom_tape_target}" PRIVATE
                    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
            endif()
            add_test(NAME "framework.ai.training.rotary_tape_${_umicom_tape_case}"
                COMMAND "${_umicom_tape_target}")
            set_tests_properties("framework.ai.training.rotary_tape_${_umicom_tape_case}"
                PROPERTIES LABELS "framework;ai;native-training;autodiff;rotary")
        endif()
    endforeach()
    add_test(NAME framework.ai.training.rotary_tape_demo
        COMMAND umicom-native-llm-rotary-tape)
    set_tests_properties(framework.ai.training.rotary_tape_demo
        PROPERTIES LABELS "framework;ai;native-training;autodiff;rotary;learning")
endif()
