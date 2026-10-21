#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeLlmTraining.cmake
#
# PURPOSE:
#   Compose the additive, Framework-owned C23 automatic differentiation
#   foundation and its focused tests. No existing AI target is replaced.
#
# LIFECYCLE:
#   This module can be included from the dedicated learning CMake project
#   immediately, or from the canonical Framework CMakeLists after review.
#   The module is include-guarded to prevent duplicate registration.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

get_filename_component(_umicom_ai_training_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET umicom_ai_training)
    add_library(umicom_ai_training STATIC
        "${_umicom_ai_training_root}/src/ai/training/tape.c"
        "${_umicom_ai_training_root}/src/ai/training/initialisation.c"
        "${_umicom_ai_training_root}/src/ai/training/sequence_shape.c"
        "${_umicom_ai_training_root}/src/ai/training/transformer.c"
        "${_umicom_ai_training_root}/src/ai/training/decoder.c"
        "${_umicom_ai_training_root}/src/ai/training/checkpoint.c"
        "${_umicom_ai_training_root}/src/ai/training/dataset_split.c"
        "${_umicom_ai_training_root}/src/ai/training/byte_tokenizer.c"
        "${_umicom_ai_training_root}/src/ai/training/corpus.c"
    )
    add_library(Umicom::AiTraining ALIAS umicom_ai_training)
    target_include_directories(umicom_ai_training PUBLIC
        "$<BUILD_INTERFACE:${_umicom_ai_training_root}/include>"
    )
    target_compile_features(umicom_ai_training PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_training PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_training PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow
        )
    endif()
    # libm is required by some Unix linkers for exp() and log().
    if(NOT MSVC AND NOT WIN32)
        target_link_libraries(umicom_ai_training PUBLIC m)
    endif()
endif()

if(BUILD_TESTING)
    function(_umicom_add_native_ai_training_test target test_name source_file)
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}"
            "${_umicom_ai_training_root}/tests/ai_training/${source_file}")
        target_link_libraries("${target}" PRIVATE Umicom::AiTraining)
        target_compile_features("${target}" PRIVATE c_std_23)
        if(MSVC)
            target_compile_options("${target}" PRIVATE /W4)
        else()
            target_compile_options("${target}" PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES
            LABELS "framework;ai;native-training")
    endfunction()

    _umicom_add_native_ai_training_test(
        umicom-ai-training-contract-test
        framework.ai.training.contract
        test_training_contract.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-gradient-test
        framework.ai.training.gradients
        test_training_gradients.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-regression-test
        framework.ai.training.regression
        test_training_regression.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-bigram-test
        framework.ai.training.bigram
        test_training_bigram.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-finite-differences-test
        framework.ai.training.finite_differences
        test_training_numerical_gradients.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-limits-test
        framework.ai.training.limits
        test_training_limits.c)

    # Batch 02 extends the existing tape rather than introducing a parallel
    # autodiff system. These tests qualify the new native operators.
    _umicom_add_native_ai_training_test(
        umicom-ai-training-initialisation-test
        framework.ai.training.initialisation
        test_training_initialisation.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-sequence-shape-test
        framework.ai.training.sequence_shape
        test_training_sequence_shape.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-embedding-test
        framework.ai.training.embedding
        test_training_embedding.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-attention-test
        framework.ai.training.attention
        test_training_attention.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-attention-gradient-test
        framework.ai.training.attention_gradients
        test_training_attention_gradients.c)

    # Batch 03: multi-head attention, trainable layer normalisation and
    # failure-atomic pre-LN transformer block composition. The original
    # twelve tests and the Umicom::ai runtime remain unchanged.
    _umicom_add_native_ai_training_test(
        umicom-ai-training-multihead-test
        framework.ai.training.multihead
        test_training_multihead.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-multihead-gradients-test
        framework.ai.training.multihead_gradients
        test_training_multihead_gradients.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-layernorm-test
        framework.ai.training.layernorm
        test_training_layernorm.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-layernorm-gradients-test
        framework.ai.training.layernorm_gradients
        test_training_layernorm_gradients.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-transformer-test
        framework.ai.training.transformer
        test_training_transformer.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-transformer-gradients-test
        framework.ai.training.transformer_gradients
        test_training_transformer_gradients.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-transformer-training-test
        framework.ai.training.transformer_training
        test_training_transformer_training.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-block-failures-test
        framework.ai.training.block_failures
        test_training_block_failures.c)
endif()

# Batch 04: one complete (bounded) decoder, held-out sequence split, versioned
# checkpoint snapshots and atomic multi-parameter SGD. Retain every earlier
# test registration and shared Framework AI target unchanged.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-sgd-transaction-test
        framework.ai.training.sgd_transaction
        test_training_sgd_transaction.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-dataset-split-test
        framework.ai.training.dataset_split
        test_training_dataset_split.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-decoder-contract-test
        framework.ai.training.decoder_contract
        test_training_decoder_contract.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-decoder-train-test
        framework.ai.training.decoder_train
        test_training_decoder_train.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-checkpoint-test
        framework.ai.training.checkpoint
        test_training_checkpoint.c)
endif()

# Batch 05: bounded, lossless byte tokenisation and local corpus ingestion.
# These are independent source-aware training prerequisites; no existing dataset,
# parameter, embedding, or transformer logic is reimplemented here.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-byte-tokenizer-test
        framework.ai.training.byte_tokenizer
        test_training_byte_tokenizer.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-corpus-test
        framework.ai.training.corpus
        test_training_corpus.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-corpus-file-test
        framework.ai.training.corpus_file
        test_training_corpus_file.c)
endif()
