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
        "${_umicom_ai_training_root}/src/ai/training/dataset_manifest.c"
        # Native experiment plans and deterministic run orchestration:
        # and sampled held-out evaluation reuse the canonical model/dataset.
        "${_umicom_ai_training_root}/src/ai/training/experiment.c"
        "${_umicom_ai_training_root}/src/ai/training/experiment_plan.c"
        # Native autoregressive generation composes the decoder and RNG.
        "${_umicom_ai_training_root}/src/ai/training/generation.c"
        # Hold-out scoring and identity evidence reuse the canonical decoder,
        # immutable source manifest, experiment plan and checkpoint codec.
        "${_umicom_ai_training_root}/src/ai/training/model_audit.c"
        # Qualified inference-only attention key/value cache. The original
        # decoder, training tape and model mathematics remain authoritative.
        "${_umicom_ai_training_root}/src/ai/training/kv_cache.c"
        # Shared scalar reference and column-tiled CPU matrix kernels.
        "${_umicom_ai_training_root}/src/ai/training/matrix_kernel.c"
        # Whole-decoder profiling and exact cache capacity accounting use the
        # established decoder, model metadata and inference-only cache.
        "${_umicom_ai_training_root}/src/ai/training/inference_profile.c"
        # Inspect actual decoder tensor shapes and trainable memory limits
        # before model creation; this does not introduce a second model.
        "${_umicom_ai_training_root}/src/ai/training/capacity_plan.c"
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

    # Native attention extends the existing tape rather than introducing a parallel
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

    # Multi-head attention, trainable layer normalisation and
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

# A complete bounded decoder, held-out sequence split, versioned
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

# Bounded, lossless byte tokenisation and local corpus ingestion.
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

# Whole-file dataset split and deterministic source-code minibatches.
# These additions use the existing tokenizer, corpus, RNG and decoder APIs.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-manifest-test
        framework.ai.training.manifest
        test_training_dataset_manifest.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-manifest-rejections-test
        framework.ai.training.manifest_rejections
        test_training_dataset_manifest_rejections.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-source-model-test
        framework.ai.training.source_model
        test_training_source_model.c)
    # Resolve fixture paths from the checked-out source directory, NEVER cwd.
    # Three fixed, licensed teaching files are used for deterministic tests.
    foreach(_source_model_test IN ITEMS
        umicom-ai-training-manifest-test
        umicom-ai-training-source-model-test)
        target_compile_definitions(${_source_model_test} PRIVATE
            UMI_AI_TEST_MANIFEST_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/manifest.txt")
    endforeach()
endif()

# Separate configuration, reproducibility and cancellation checks.
# The tests use immutable authored fixtures and never publish checkpoints.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-experiment-plan-test
        framework.ai.training.experiment_plan
        test_training_experiment_plan.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-experiment-run-test
        framework.ai.training.experiment_run
        test_training_experiment_run.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-experiment-cancel-test
        framework.ai.training.experiment_cancel
        test_training_experiment_cancel.c)
    foreach(_experiment_test IN ITEMS
        umicom-ai-training-experiment-run-test
        umicom-ai-training-experiment-cancel-test)
        target_compile_definitions(${_experiment_test} PRIVATE
            UMI_AI_TEST_MANIFEST_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/manifest.txt")
    endforeach()
    target_compile_definitions(umicom-ai-training-experiment-run-test PRIVATE
        UMI_AI_TEST_EXPERIMENT_PLAN_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/experiment.conf")
endif()

# Validate deterministic top-K selection, bounded byte generation and
# checkpoint-stable inference independently of model training experiments.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-sampling-test
        framework.ai.training.sampling
        test_training_sampling.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-generation-test
        framework.ai.training.generation
        test_training_generation.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-generation-bytes-test
        framework.ai.training.generation_bytes
        test_training_generation_bytes.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-generation-checkpoint-test
        framework.ai.training.generation_checkpoint
        test_training_generation_checkpoint.c)
endif()

# Independent evidence checks protect attribution of evaluation metrics to the
# approved model checkpoint, source files and training plan.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-model-audit-test
        framework.ai.training.model_audit
        test_training_model_audit.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-audit-changes-test
        framework.ai.training.audit_changes
        test_training_audit_changes.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-audit-rejections-test
        framework.ai.training.audit_rejections
        test_training_audit_rejections.c)
    foreach(_audit_test IN ITEMS
        umicom-ai-training-model-audit-test
        umicom-ai-training-audit-changes-test
        umicom-ai-training-audit-rejections-test)
        target_compile_definitions(${_audit_test} PRIVATE
            UMI_AI_TEST_MANIFEST_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/manifest.txt")
    endforeach()
    target_compile_definitions(umicom-ai-training-audit-changes-test PRIVATE
        UMI_AI_TEST_ALTERNATIVE_MANIFEST_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/reviewed-alternative-manifest.txt")
endif()

# Key/value cache parity is checked against canonical decoder logits for
# warm prefixes, rolling learned-position windows, training and checkpoints.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-kv-cache-parity-test
        framework.ai.training.kv_cache_parity
        test_training_kv_cache_parity.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-kv-cache-rejection-test
        framework.ai.training.kv_cache_rejections
        test_training_kv_cache_rejection.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-kv-cache-weights-test
        framework.ai.training.kv_cache_weights
        test_training_kv_cache_weights.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-kv-cache-context-test
        framework.ai.training.kv_cache_context
        test_training_kv_cache_context.c)
endif()

# Validate the reusable matrix kernel before qualifying accelerated backends.
# Training and KV-cache consumers use the same bounded C23 implementation.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-matrix-contract-test
        framework.ai.training.matrix_contract
        test_training_matrix_contract.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-matrix-tape-test
        framework.ai.training.matrix_tape
        test_training_matrix_tape.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-matrix-inference-test
        framework.ai.training.matrix_inference
        test_training_matrix_inference.c)
endif()

# Profile complete decoder and cached inference on identical token streams.
# Deterministic work counts and numerical parity are test gates; CPU timings
# are observations only and cannot make a regression test fail.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-inference-footprint-test
        framework.ai.training.inference_footprint
        test_training_inference_footprint.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-inference-parity-test
        framework.ai.training.inference_parity
        test_training_inference_parity.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-inference-rejections-test
        framework.ai.training.inference_rejections
        test_training_inference_rejections.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-inference-checkpoint-test
        framework.ai.training.inference_checkpoint
        test_training_inference_checkpoint.c)
endif()

# Exact shape-sized inference allocations preserve decoder parity and public
# lifetime, while avoiding maximum-capacity storage per trained parameter.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-compact-cache-layout-test
        framework.ai.training.compact_cache_layout
        test_training_cache_compact_layout.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-compact-cache-state-test
        framework.ai.training.compact_cache_state
        test_training_cache_compact_state.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-compact-cache-lifetime-test
        framework.ai.training.compact_cache_lifetime
        test_training_cache_compact_lifetime.c)
endif()

# Observe committed training-tape memory, enforce explicit graph budgets and
# check decoder capacity against the existing authoritative tensor limits.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-memory-budget-test
        framework.ai.training.memory_budget
        test_training_memory_budget.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-memory-auxiliary-test
        framework.ai.training.memory_auxiliary
        test_training_memory_auxiliary.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-decoder-memory-test
        framework.ai.training.decoder_memory
        test_training_decoder_memory.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-capacity-plan-test
        framework.ai.training.capacity_plan
        test_training_capacity_plan.c)
endif()
