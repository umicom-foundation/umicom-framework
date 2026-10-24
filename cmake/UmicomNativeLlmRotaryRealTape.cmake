#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeLlmRotaryRealTape.cmake
# PURPOSE:
#   Register opt-in, production-linked C23 tape acceptance tests. No substitute
#   model/tape implementation is compiled into the three executables.
#   Existing training target, optimiser, RoPE maths and test names are retained.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umi_real_tape_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET umicom_ai_training OR NOT TARGET umicom-native-llm-rotary-qualification)
    message(FATAL_ERROR
        "Register UmicomNativeLlmTraining, RotaryTape and RotaryQualification modules before RotaryRealTape")
endif()

# These targets link the actual production tape.c via Umicom::AiTraining.
# The independent finite-difference oracle is a TEST-ONLY executable and
# never becomes a second Framework inference or training implementation.
if(BUILD_TESTING)
    foreach(_umi_real_case IN ITEMS gradients budget rewind)
        set(_umi_real_target "umicom-ai-training-rotary-real-tape-${_umi_real_case}-test")
        if(NOT TARGET "${_umi_real_target}")
            add_executable("${_umi_real_target}"
                "${_umi_real_tape_root}/tests/ai_training/test_training_rotary_real_tape_${_umi_real_case}.c")
            target_link_libraries("${_umi_real_target}" PRIVATE Umicom::AiTraining)
            target_compile_features("${_umi_real_target}" PRIVATE c_std_23)
            if(MSVC)
                target_compile_options("${_umi_real_target}" PRIVATE /W4)
            else()
                target_compile_options("${_umi_real_target}" PRIVATE
                    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
            endif()
            add_test(NAME "framework.ai.training.rotary_real_tape_${_umi_real_case}"
                     COMMAND "${_umi_real_target}")
            set_tests_properties(
                "framework.ai.training.rotary_real_tape_${_umi_real_case}"
                PROPERTIES LABELS "framework;ai;native-training;rotary;real-tape;production-integration")
        endif()
    endforeach()
endif()

if(NOT TARGET umicom-native-llm-real-rotary-tape)
    add_executable(umicom-native-llm-real-rotary-tape
        "${_umi_real_tape_root}/learning/native-llm/prefix-work/inspect_real_rotary_tape.c")
    target_link_libraries(umicom-native-llm-real-rotary-tape PRIVATE Umicom::AiTraining)
    target_compile_features(umicom-native-llm-real-rotary-tape PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-native-llm-real-rotary-tape PRIVATE /W4)
    else()
        target_compile_options(umicom-native-llm-real-rotary-tape PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    if(BUILD_TESTING)
        add_test(NAME framework.ai.training.rotary_real_tape_demo
                 COMMAND umicom-native-llm-real-rotary-tape)
        set_tests_properties(framework.ai.training.rotary_real_tape_demo
            PROPERTIES LABELS "framework;ai;native-training;rotary;real-tape;learning")
    endif()
endif()
