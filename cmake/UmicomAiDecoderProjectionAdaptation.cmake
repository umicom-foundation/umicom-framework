#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiDecoderProjectionAdaptation.cmake
# PURPOSE: Compose native projection-level decoder adaptation from the existing
#          decoder, training-tape, low-rank and selected-token-loss services.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

get_filename_component(UMICOM_AI_DECODER_ADAPTATION_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

# Keep existing Framework and adaptation targets as their sole mathematical
# owners. This module adds one optional orchestration library, not another
# Transformer, neural optimiser, checkpoint codec or application runtime.
if(NOT TARGET Umicom::AiAdaptation)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiAdaptation.cmake")
endif()

if(NOT TARGET umicom_ai_decoder_projection_adaptation)
    add_library(umicom_ai_decoder_projection_adaptation STATIC
        "${UMICOM_AI_DECODER_ADAPTATION_ROOT}/src/ai/training/decoder_projection_adapter.c")
    add_library(Umicom::AiDecoderProjectionAdaptation
        ALIAS umicom_ai_decoder_projection_adaptation)
    target_include_directories(umicom_ai_decoder_projection_adaptation PUBLIC
        "${UMICOM_AI_DECODER_ADAPTATION_ROOT}/include")
    target_link_libraries(umicom_ai_decoder_projection_adaptation PUBLIC
        Umicom::AiAdaptation)
    target_compile_features(umicom_ai_decoder_projection_adaptation PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_decoder_projection_adaptation PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_decoder_projection_adaptation PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

if(BUILD_TESTING AND NOT TARGET umicom-ai-training-decoder-projection-adapter-test)
    add_executable(umicom-ai-training-decoder-projection-adapter-test
        "${UMICOM_AI_DECODER_ADAPTATION_ROOT}/tests/ai_training/test_training_decoder_projection_adapter.c")
    target_link_libraries(umicom-ai-training-decoder-projection-adapter-test PRIVATE
        Umicom::AiDecoderProjectionAdaptation)
    target_compile_features(umicom-ai-training-decoder-projection-adapter-test PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-ai-training-decoder-projection-adapter-test PRIVATE /W4)
    else()
        target_compile_options(umicom-ai-training-decoder-projection-adapter-test PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    add_test(NAME framework.ai.adaptation.decoder_projection
        COMMAND umicom-ai-training-decoder-projection-adapter-test)
    set_tests_properties(framework.ai.adaptation.decoder_projection PROPERTIES
        LABELS "framework;ai;native-training;adaptation")
endif()
