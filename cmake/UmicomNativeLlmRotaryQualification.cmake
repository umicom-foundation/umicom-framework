#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeLlmRotaryQualification.cmake
# PURPOSE:
#   Compile independent numerical and lifetime acceptance tests against the
#   existing real Framework training target. Test-only math is NOT added to
#   production libraries or duplicated in application code.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umi_rope_qualification_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET umicom_ai_training OR
   NOT TARGET umicom-native-llm-rotary-optimisation)
    message(FATAL_ERROR
        "Native LLM training, rotary research, rotary tape and optimisation modules must be included first")
endif()

add_executable(umicom-native-llm-rotary-qualification
    "${_umi_rope_qualification_root}/learning/native-llm/prefix-work/audit_rotary_training.c")
target_link_libraries(umicom-native-llm-rotary-qualification PRIVATE Umicom::AiTraining)
target_compile_features(umicom-native-llm-rotary-qualification PRIVATE c_std_23)
if(MSVC)
    target_compile_options(umicom-native-llm-rotary-qualification PRIVATE /W4)
else()
    target_compile_options(umicom-native-llm-rotary-qualification PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()

if(BUILD_TESTING)
    foreach(_umi_rope_case IN ITEMS oracle lifecycle)
        set(_umi_rope_target "umicom-ai-training-rotary-qualification-${_umi_rope_case}-test")
        add_executable("${_umi_rope_target}"
            "${_umi_rope_qualification_root}/tests/ai_training/test_training_rotary_qualification_${_umi_rope_case}.c")
        target_link_libraries("${_umi_rope_target}" PRIVATE Umicom::AiTraining)
        target_compile_features("${_umi_rope_target}" PRIVATE c_std_23)
        if(MSVC)
            target_compile_options("${_umi_rope_target}" PRIVATE /W4)
        else()
            target_compile_options("${_umi_rope_target}" PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
        endif()
        add_test(NAME "framework.ai.training.rotary_qualification_${_umi_rope_case}"
            COMMAND "${_umi_rope_target}")
        set_tests_properties(
            "framework.ai.training.rotary_qualification_${_umi_rope_case}"
            PROPERTIES LABELS "framework;ai;native-training;rotary;gradient;production-tape-integration")
    endforeach()
    add_test(NAME framework.ai.training.rotary_qualification_demo
        COMMAND umicom-native-llm-rotary-qualification)
    set_tests_properties(framework.ai.training.rotary_qualification_demo
        PROPERTIES LABELS "framework;ai;native-training;rotary;learning")
endif()
