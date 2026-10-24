#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeLlmRotaryOptimisation.cmake
# PURPOSE:
#   Add a bounded, native C23 RoPE optimisation laboratory on top of the
#   original Framework training tape, rotary bridge and SGD implementation.
#   This file is additive: it never replaces a historic CMake definition.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_rotary_optimisation_root
    "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET umicom_ai_training)
    message(FATAL_ERROR "Include UmicomNativeLlmTraining.cmake first")
endif()
if(NOT TARGET umicom-native-llm-rotary-tape)
    message(FATAL_ERROR "Include UmicomNativeLlmRotaryTape.cmake first")
endif()

target_sources(umicom_ai_training PRIVATE
    "${_umicom_rotary_optimisation_root}/src/ai/training/rotary_optimisation.c")

if(NOT TARGET umicom-native-llm-rotary-optimisation)
    add_executable(umicom-native-llm-rotary-optimisation
        "${_umicom_rotary_optimisation_root}/learning/native-llm/prefix-work/train_rotary_attention.c")
    target_link_libraries(umicom-native-llm-rotary-optimisation PRIVATE
        Umicom::AiTraining)
    target_compile_features(umicom-native-llm-rotary-optimisation PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-native-llm-rotary-optimisation PRIVATE /W4)
    else()
        target_compile_options(umicom-native-llm-rotary-optimisation PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

if(BUILD_TESTING)
    foreach(_umicom_rotary_opt_case IN ITEMS convergence rejections)
        set(_umicom_rotary_opt_target
            "umicom-ai-training-rotary-optimisation-${_umicom_rotary_opt_case}-test")
        if(NOT TARGET "${_umicom_rotary_opt_target}")
            add_executable("${_umicom_rotary_opt_target}"
                "${_umicom_rotary_optimisation_root}/tests/ai_training/test_training_rotary_optimisation_${_umicom_rotary_opt_case}.c")
            target_link_libraries("${_umicom_rotary_opt_target}" PRIVATE
                Umicom::AiTraining)
            target_compile_features("${_umicom_rotary_opt_target}" PRIVATE c_std_23)
            if(MSVC)
                target_compile_options("${_umicom_rotary_opt_target}" PRIVATE /W4)
            else()
                target_compile_options("${_umicom_rotary_opt_target}" PRIVATE
                    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
            endif()
            add_test(NAME "framework.ai.training.rotary_optimisation_${_umicom_rotary_opt_case}"
                COMMAND "${_umicom_rotary_opt_target}")
            set_tests_properties("framework.ai.training.rotary_optimisation_${_umicom_rotary_opt_case}"
                PROPERTIES LABELS "framework;ai;native-training;rotary;optimiser;education")
        endif()
    endforeach()
    add_test(NAME framework.ai.training.rotary_optimisation_demo
        COMMAND umicom-native-llm-rotary-optimisation)
    set_tests_properties(framework.ai.training.rotary_optimisation_demo
        PROPERTIES LABELS "framework;ai;native-training;rotary;learning")
endif()
