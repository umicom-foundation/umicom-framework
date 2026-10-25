#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiAdaptation.cmake
# PURPOSE: Reuse native tensor training and attach reviewed source admission
#          and trainable low-rank projections without duplicating a decoder.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

get_filename_component(UMICOM_AI_ADAPTATION_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET Umicom::AiTraining)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomNativeLlmTraining.cmake")
endif()

if(NOT TARGET umicom_ai_adaptation)
    add_library(umicom_ai_adaptation STATIC
        "${UMICOM_AI_ADAPTATION_ROOT}/src/ai/training/low_rank.c"
        "${UMICOM_AI_ADAPTATION_ROOT}/src/ai/training/source_admission.c"
        "${UMICOM_AI_ADAPTATION_ROOT}/src/ai/training/caption_text.c")
    add_library(Umicom::AiAdaptation ALIAS umicom_ai_adaptation)
    target_include_directories(umicom_ai_adaptation PUBLIC
        "${UMICOM_AI_ADAPTATION_ROOT}/include")
    target_link_libraries(umicom_ai_adaptation PUBLIC Umicom::AiTraining)
    # Subtitle decoding remains owned by the pre-existing Creative Workspace.
    if(TARGET Umicom::creative_workspace)
        target_link_libraries(umicom_ai_adaptation PUBLIC Umicom::creative_workspace)
    endif()
    target_compile_features(umicom_ai_adaptation PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_adaptation PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_adaptation PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

if(BUILD_TESTING)
    if(NOT TARGET umicom-ai-training-low-rank-test)
        add_executable(umicom-ai-training-low-rank-test
            "${UMICOM_AI_ADAPTATION_ROOT}/tests/ai_training/test_training_low_rank.c")
        target_link_libraries(umicom-ai-training-low-rank-test PRIVATE
            Umicom::AiAdaptation)
        add_test(NAME framework.ai.adaptation.low_rank
            COMMAND umicom-ai-training-low-rank-test)
    endif()
    if(NOT TARGET umicom-ai-training-source-admission-test)
        add_executable(umicom-ai-training-source-admission-test
            "${UMICOM_AI_ADAPTATION_ROOT}/tests/ai_training/test_training_source_admission.c")
        target_link_libraries(umicom-ai-training-source-admission-test PRIVATE
            Umicom::AiAdaptation)
        add_test(NAME framework.ai.adaptation.source_admission
            COMMAND umicom-ai-training-source-admission-test)
    endif()
    if(TARGET Umicom::creative_workspace AND NOT TARGET umicom-ai-training-caption-text-test)
        add_executable(umicom-ai-training-caption-text-test
            "${UMICOM_AI_ADAPTATION_ROOT}/tests/ai_training/test_training_caption_text.c")
        target_link_libraries(umicom-ai-training-caption-text-test PRIVATE
            Umicom::AiAdaptation Umicom::creative_workspace)
        add_test(NAME framework.ai.adaptation.caption_text
            COMMAND umicom-ai-training-caption-text-test)
    endif()
    foreach(target IN ITEMS
            umicom-ai-training-low-rank-test
            umicom-ai-training-source-admission-test)
        target_compile_features(${target} PRIVATE c_std_23)
        if(MSVC)
            target_compile_options(${target} PRIVATE /W4)
        else()
            target_compile_options(${target} PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
    endforeach()
endif()
