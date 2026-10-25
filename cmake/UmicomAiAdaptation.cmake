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
        "${UMICOM_AI_ADAPTATION_ROOT}/src/ai/training/transformer_low_rank.c"
        "${UMICOM_AI_ADAPTATION_ROOT}/src/ai/training/source_admission.c"
        "${UMICOM_AI_ADAPTATION_ROOT}/src/ai/training/caption_text.c"
        "${UMICOM_AI_ADAPTATION_ROOT}/src/ai/training/adapter_checkpoint.c"
        "${UMICOM_AI_ADAPTATION_ROOT}/src/ai/training/source_evidence.c"
        "${UMICOM_AI_ADAPTATION_ROOT}/src/ai/training/low_rank_training.c"
        "${UMICOM_AI_ADAPTATION_ROOT}/src/ai/training/reviewed_examples.c"
        "${UMICOM_AI_ADAPTATION_ROOT}/src/ai/training/decoder_logit_adapter.c"
        "${UMICOM_AI_ADAPTATION_ROOT}/src/ai/training/reviewed_corpus.c"
        # Reuse the canonical byte tokenizer and cross-entropy; no new neural
        # maths, curriculum-local runtime or application-specific trainer.
        "${UMICOM_AI_ADAPTATION_ROOT}/src/ai/training/supervised_window.c"
        "${UMICOM_AI_ADAPTATION_ROOT}/src/ai/training/selected_loss.c")
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
    if(NOT TARGET umicom-ai-training-adapter-checkpoint-test)
        add_executable(umicom-ai-training-adapter-checkpoint-test
            "${UMICOM_AI_ADAPTATION_ROOT}/tests/ai_training/test_training_adapter_checkpoint.c")
        target_link_libraries(umicom-ai-training-adapter-checkpoint-test PRIVATE
            Umicom::AiAdaptation)
        add_test(NAME framework.ai.adaptation.adapter_checkpoint
            COMMAND umicom-ai-training-adapter-checkpoint-test)
    endif()
    if(NOT TARGET umicom-ai-training-source-evidence-test)
        add_executable(umicom-ai-training-source-evidence-test
            "${UMICOM_AI_ADAPTATION_ROOT}/tests/ai_training/test_training_source_evidence.c")
        target_link_libraries(umicom-ai-training-source-evidence-test PRIVATE
            Umicom::AiAdaptation)
        add_test(NAME framework.ai.adaptation.source_evidence
            COMMAND umicom-ai-training-source-evidence-test)
    endif()
    # A supervised update uses the existing Framework graph and SGD writer.
    if(NOT TARGET umicom-ai-training-low-rank-step-test)
        add_executable(umicom-ai-training-low-rank-step-test
            "${UMICOM_AI_ADAPTATION_ROOT}/tests/ai_training/test_training_low_rank_step.c")
        target_link_libraries(umicom-ai-training-low-rank-step-test PRIVATE
            Umicom::AiAdaptation)
        add_test(NAME framework.ai.adaptation.low_rank_step
            COMMAND umicom-ai-training-low-rank-step-test)
    endif()
    # Review and actual SHA-256 checks precede JSONL preparation.
    if(NOT TARGET umicom-ai-training-reviewed-examples-test)
        add_executable(umicom-ai-training-reviewed-examples-test
            "${UMICOM_AI_ADAPTATION_ROOT}/tests/ai_training/test_training_reviewed_examples.c")
        target_link_libraries(umicom-ai-training-reviewed-examples-test PRIVATE
            Umicom::AiAdaptation)
        add_test(NAME framework.ai.adaptation.reviewed_examples
            COMMAND umicom-ai-training-reviewed-examples-test)
    endif()
    # Use real frozen-decoder logits and the canonical token-level loss.
    if(NOT TARGET umicom-ai-training-decoder-logit-adapter-test)
        add_executable(umicom-ai-training-decoder-logit-adapter-test
            "${UMICOM_AI_ADAPTATION_ROOT}/tests/ai_training/test_training_decoder_logit_adapter.c")
        target_link_libraries(umicom-ai-training-decoder-logit-adapter-test PRIVATE
            Umicom::AiAdaptation)
        add_test(NAME framework.ai.adaptation.decoder_logit_adapter
            COMMAND umicom-ai-training-decoder-logit-adapter-test)
    endif()
    # Source-group split checks run over reviewed, byte-verified JSONL records.
    if(NOT TARGET umicom-ai-training-reviewed-corpus-test)
        add_executable(umicom-ai-training-reviewed-corpus-test
            "${UMICOM_AI_ADAPTATION_ROOT}/tests/ai_training/test_training_reviewed_corpus.c")
        target_link_libraries(umicom-ai-training-reviewed-corpus-test PRIVATE
            Umicom::AiAdaptation)
        add_test(NAME framework.ai.adaptation.reviewed_corpus
            COMMAND umicom-ai-training-reviewed-corpus-test)
    endif()
    # Optional projection hooks preserve the existing Transformer block path.
    if(NOT TARGET umicom-ai-training-transformer-low-rank-test)
        add_executable(umicom-ai-training-transformer-low-rank-test
            "${UMICOM_AI_ADAPTATION_ROOT}/tests/ai_training/test_training_transformer_low_rank.c")
        target_link_libraries(umicom-ai-training-transformer-low-rank-test PRIVATE
            Umicom::AiAdaptation)
        add_test(NAME framework.ai.adaptation.transformer_low_rank
            COMMAND umicom-ai-training-transformer-low-rank-test)
    endif()
    if(NOT TARGET umicom-ai-training-supervised-window-test)
        add_executable(umicom-ai-training-supervised-window-test
            "${UMICOM_AI_ADAPTATION_ROOT}/tests/ai_training/test_training_supervised_window.c")
        target_link_libraries(umicom-ai-training-supervised-window-test PRIVATE
            Umicom::AiAdaptation)
        add_test(NAME framework.ai.adaptation.supervised_window
            COMMAND umicom-ai-training-supervised-window-test)
    endif()
    if(NOT TARGET umicom-ai-training-selected-loss-test)
        add_executable(umicom-ai-training-selected-loss-test
            "${UMICOM_AI_ADAPTATION_ROOT}/tests/ai_training/test_training_selected_loss.c")
        target_link_libraries(umicom-ai-training-selected-loss-test PRIVATE
            Umicom::AiAdaptation)
        add_test(NAME framework.ai.adaptation.selected_loss
            COMMAND umicom-ai-training-selected-loss-test)
    endif()
    if(NOT TARGET umicom-ai-training-decoder-selected-test)
        add_executable(umicom-ai-training-decoder-selected-test
            "${UMICOM_AI_ADAPTATION_ROOT}/tests/ai_training/test_training_decoder_selected.c")
        target_link_libraries(umicom-ai-training-decoder-selected-test PRIVATE
            Umicom::AiAdaptation)
        add_test(NAME framework.ai.adaptation.decoder_selected
            COMMAND umicom-ai-training-decoder-selected-test)
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
            umicom-ai-training-source-admission-test
            umicom-ai-training-adapter-checkpoint-test
            umicom-ai-training-source-evidence-test
            umicom-ai-training-low-rank-step-test
            umicom-ai-training-reviewed-examples-test
            umicom-ai-training-decoder-logit-adapter-test
            umicom-ai-training-reviewed-corpus-test
            umicom-ai-training-transformer-low-rank-test
            umicom-ai-training-supervised-window-test
            umicom-ai-training-selected-loss-test
            umicom-ai-training-decoder-selected-test)
        target_compile_features(${target} PRIVATE c_std_23)
        if(MSVC)
            target_compile_options(${target} PRIVATE /W4)
        else()
            target_compile_options(${target} PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
    endforeach()
endif()
