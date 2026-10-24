#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeLlmRotaryAcceptance.cmake
# PURPOSE: Opt-in admission for externally observed real-tape test evidence.
# Does not replace existing CTest, tape mathematics, or test result authority.
# AUTHOR: Sammy Hegab | Umicom Foundation | MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umi_acceptance_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET umicom_ai_training)
    message(FATAL_ERROR "Include UmicomNativeLlmTraining.cmake first")
endif()
target_sources(umicom_ai_training PRIVATE
    "${_umi_acceptance_root}/src/ai/training/rotary_acceptance.c")
if(BUILD_TESTING AND NOT TARGET umicom-ai-training-rotary-acceptance-test)
    add_executable(umicom-ai-training-rotary-acceptance-test
        "${_umi_acceptance_root}/tests/ai_training/test_training_rotary_acceptance.c")
    target_link_libraries(umicom-ai-training-rotary-acceptance-test PRIVATE Umicom::AiTraining)
    target_compile_features(umicom-ai-training-rotary-acceptance-test PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-ai-training-rotary-acceptance-test PRIVATE /W4)
    else()
        target_compile_options(umicom-ai-training-rotary-acceptance-test PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    add_test(NAME framework.ai.training.rotary_acceptance
             COMMAND umicom-ai-training-rotary-acceptance-test)
    set_tests_properties(framework.ai.training.rotary_acceptance
        PROPERTIES LABELS "framework;ai;native-training;acceptance;fixture-only")
endif()
