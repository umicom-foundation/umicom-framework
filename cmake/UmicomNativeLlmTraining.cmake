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
        # Stateful AdamW and absolute-step learning rates reuse the existing
        # training tape as the sole authority for parameter updates.
        "${_umicom_ai_training_root}/src/ai/training/optimizer.c"
        "${_umicom_ai_training_root}/src/ai/training/gradient_clipping.c"
        # Reviewed experiment policy records bind exact clipping settings to
        # immutable AdamW checkpoint bytes using the existing base SHA-256.
        "${_umicom_ai_training_root}/src/ai/training/gradient_policy.c"
        # Independent continuation identity preserves the original checkpoint and
        # clipping record, adding strict opt-in dataset/schedule/AdamW binding.
        "${_umicom_ai_training_root}/src/ai/training/training_run_lock.c"
        # Optional cryptographic source-snapshot lock. This hashes the exact
        # already-imported immutable corpus bytes; no second data loader.
        "${_umicom_ai_training_root}/src/ai/training/training_source_lock.c"
        # Optional completed-bundle integrity marker: stream SHA-256 over
        # the saved AdamW checkpoint and all three existing policy sidecars.
        # The marker is strictly supplementary and is always verified on read.
        "${_umicom_ai_training_root}/src/ai/training/training_publication.c"
        # Exact resume of combined decoder + AdamW moments. The existing
        # standalone decoder checkpoint remains byte-for-byte unchanged.
        "${_umicom_ai_training_root}/src/ai/training/optimizer_checkpoint.c"
        # Reproducible AdamW experiments clone combined optimiser checkpoints
        # before resuming; original SGD experiments remain untouched.
        "${_umicom_ai_training_root}/src/ai/training/experiment_adamw.c"
        "${_umicom_ai_training_root}/src/ai/training/experiment_plan.c"
        # Native autoregressive generation composes the decoder and RNG.
        "${_umicom_ai_training_root}/src/ai/training/generation.c"
        # Qualified KV cache orchestration adds no new model or sampling maths.
        "${_umicom_ai_training_root}/src/ai/training/generation_cache.c"
        # Retained streaming state composes the same KV cache and top-p sampler.
        "${_umicom_ai_training_root}/src/ai/training/generation_session.c"
        # Independently measurable inference economics: existing decoder and
        # cache/session APIs remain sole mathematics and allocation authorities.
        "${_umicom_ai_training_root}/src/ai/training/generation_performance.c"
        # Independent, byte-exact completion evidence reuses the saved model
        # and the canonical native sampler; generated code is never executed.
        "${_umicom_ai_training_root}/src/ai/training/benchmark.c"
        # Bounded reviewed-suite parser: no scripts, compiler or source execution.
        "${_umicom_ai_training_root}/src/ai/training/benchmark_suite.c"
        # Bind completion evidence to portable model bytes, ordered cases
        # and observed score fields through the existing Framework SHA-256.
        "${_umicom_ai_training_root}/src/ai/training/benchmark_evidence.c"
        # Hold-out scoring and identity evidence reuse the canonical decoder,
        # immutable source manifest, experiment plan and checkpoint codec.
        "${_umicom_ai_training_root}/src/ai/training/model_audit.c"
        # Read-only candidate-quality selection reuses canonical held-out
        # ModelAuditEvaluate. It cannot authorise a signed release.
        "${_umicom_ai_training_root}/src/ai/training/model_quality_gate.c"
        # Deterministic JSON quality evidence reuses the same validated
        # comparison; binary64 metrics are exact bit strings, not locale
        # dependent decimal data or signed release attestations.
        "${_umicom_ai_training_root}/src/ai/training/model_quality_evidence.c"
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
    # Reuse the canonical base SHA-256 owner when the full Framework build
    # provides it. The isolated teaching project compiles the SAME source.
    if(TARGET Umicom::base)
        target_link_libraries(umicom_ai_training PRIVATE Umicom::base)
    else()
        target_sources(umicom_ai_training PRIVATE
            "${_umicom_ai_training_root}/src/base/sha256.c")
    endif()
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
    # Regression coverage for the opt-in full-context KV cache generation
    # adapter. Both test targets link the actual Framework inference owners.
    _umicom_add_native_ai_training_test(
        umicom-ai-training-generation-cache-parity-test
        framework.ai.training.generation_cache_parity
        test_training_generation_cache_parity.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-generation-cache-rejections-test
        framework.ai.training.generation_cache_rejections
        test_training_generation_cache_rejections.c)
    # Streaming pulls preserve seed/cache state and validate stale ownership.
    _umicom_add_native_ai_training_test(
        umicom-ai-training-generation-session-parity-test
        framework.ai.training.generation_session_parity
        test_training_generation_session_parity.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-generation-session-rejections-test
        framework.ai.training.generation_session_rejections
        test_training_generation_session_rejections.c)
    # A real-model comparison tests honest cache row forecasts against the
    # decoder, while refusal tests protect failure-atomic report publication.
    _umicom_add_native_ai_training_test(
        umicom-ai-training-generation-performance-test
        framework.ai.training.generation_performance
        test_training_generation_performance.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-generation-performance-rejections-test
        framework.ai.training.generation_performance_rejections
        test_training_generation_performance_rejections.c)
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

# The quality-comparison CLI loads two reviewed combined AdamW checkpoints
# and delegates ALL held-out model evaluation and score selection to the
# Framework. It does not sign, publish or implicitly approve a release.
if(NOT TARGET umicom-native-llm-quality-compare)
    add_executable(umicom-native-llm-quality-compare
        "${_umicom_ai_training_root}/learning/native-llm/compare_model_quality.c")
    target_link_libraries(umicom-native-llm-quality-compare PRIVATE
        Umicom::AiTraining)
    target_compile_features(umicom-native-llm-quality-compare PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-native-llm-quality-compare PRIVATE /W4)
    else()
        target_compile_options(umicom-native-llm-quality-compare PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

# Native quality gates are tested both with explicit copied metric fixtures
# and using the actual decoder, immutable corpus and canonical ModelAudit.
if(BUILD_TESTING)
    # Original registration list (all previous tests remain enabled):
    # foreach(_umicom_quality_case IN ITEMS reports rejections integration)
    # Extend it additively with output-evidence and rejection tests.
    foreach(_umicom_quality_case IN ITEMS
        reports rejections integration evidence evidence_rejections)
        _umicom_add_native_ai_training_test(
            "umicom-ai-training-model-quality-${_umicom_quality_case}-test"
            "framework.ai.training.model_quality_${_umicom_quality_case}"
            "test_training_model_quality_${_umicom_quality_case}.c")
    endforeach()
    target_compile_definitions(
        umicom-ai-training-model-quality-integration-test PRIVATE
        UMI_AI_TEST_MANIFEST_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/manifest.txt")
    # A wrong pointer type in this integration test previously prevented
    # Windows UCRT64 builds. Require a hard compiler diagnostic for the actual
    # public decoder interface, not a substitute isolated test header.
    if(MSVC)
        target_compile_options(
            umicom-ai-training-model-quality-integration-test PRIVATE /we4133)
    elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            umicom-ai-training-model-quality-integration-test PRIVATE
            -Werror=incompatible-pointer-types)
    endif()
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

# Larger bounded tensors and shape-sized matrix workspaces permit several
# independent source sequences per optimiser step. These tests retain the
# established decoder, checkpoint and training-tape implementations.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-wide-matrix-test
        framework.ai.training.wide_matrix
        test_training_wide_matrix.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-wide-tape-test
        framework.ai.training.wide_tape
        test_training_wide_tape.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-wide-decoder-test
        framework.ai.training.wide_decoder
        test_training_wide_decoder.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-multi-sequence-test
        framework.ai.training.multi_sequence
        test_training_multi_sequence.c)
    target_compile_definitions(umicom-ai-training-multi-sequence-test PRIVATE
        UMI_AI_TEST_MANIFEST_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/manifest.txt")
endif()

# Microbatch gradient accumulation shares the canonical decoder graph and
# tape-owned atomic SGD. Existing training targets remain registered.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-supplied-gradients-test
        framework.ai.training.supplied_gradients
        test_training_supplied_gradients.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-gradient-accumulation-test
        framework.ai.training.gradient_accumulation
        test_training_gradient_accumulation.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-accumulated-limits-test
        framework.ai.training.accumulated_limits
        test_training_accumulated_limits.c)
endif()

# Opt-in effective-batch experiments retain the established plan and decoder
# ABI. Deterministic dataset draws and microbatch reduction are qualified here.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-accumulation-plan-test
        framework.ai.training.accumulation_plan
        test_training_accumulation_plan.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-accumulation-run-test
        framework.ai.training.accumulation_run
        test_training_accumulation_run.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-accumulation-rejections-test
        framework.ai.training.accumulation_rejections
        test_training_accumulation_rejections.c)
    foreach(_accumulation_test IN ITEMS
        umicom-ai-training-accumulation-plan-test
        umicom-ai-training-accumulation-run-test
        umicom-ai-training-accumulation-rejections-test)
        target_compile_definitions(${_accumulation_test} PRIVATE
            UMI_AI_TEST_ACCUMULATED_PLAN_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/accumulated-experiment.conf"
            UMI_AI_TEST_MANIFEST_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/manifest.txt")
    endforeach()
endif()

# Qualify stateful AdamW updates, absolute-step learning-rate schedules and
# scheduled direct/effective-batch experiments without altering legacy SGD.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-learning-rate-test
        framework.ai.training.learning_rate
        test_training_learning_rate.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-adamw-test
        framework.ai.training.adamw
        test_training_adamw.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-decoder-adamw-test
        framework.ai.training.decoder_adamw
        test_training_decoder_adamw.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-scheduled-experiment-test
        framework.ai.training.scheduled_experiment
        test_training_scheduled_experiment.c)
    target_compile_definitions(umicom-ai-training-scheduled-experiment-test PRIVATE
        UMI_AI_TEST_MANIFEST_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/manifest.txt")
endif()

# Combined decoder/AdamW checkpoint regression tests. Preserve the existing
# experiment CLI CTest: Windows security software may independently quarantine
# that binary, which must be diagnosed rather than concealed or bypassed.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-adamw-resume-test
        framework.ai.training.adamw_resume
        test_training_adamw_resume.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-adamw-checkpoint-rejection-test
        framework.ai.training.adamw_checkpoint_rejections
        test_training_adamw_checkpoint_rejections.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-adamw-checkpoint-file-test
        framework.ai.training.adamw_checkpoint_file
        test_training_adamw_checkpoint_file.c)
endif()

# Scheduled AdamW experiments: exact model+optimiser continuation without
# disabling or renaming the established accumulation_demo regression.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-adamw-experiment-resume-test
        framework.ai.training.adamw_experiment_resume
        test_training_adamw_experiment_resume.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-adamw-experiment-contract-test
        framework.ai.training.adamw_experiment_contract
        test_training_adamw_experiment_contract.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-adamw-experiment-cancel-test
        framework.ai.training.adamw_experiment_cancel
        test_training_adamw_experiment_cancel.c)
    foreach(_adamw_experiment_test IN ITEMS
        umicom-ai-training-adamw-experiment-resume-test
        umicom-ai-training-adamw-experiment-contract-test
        umicom-ai-training-adamw-experiment-cancel-test)
        target_compile_definitions(${_adamw_experiment_test} PRIVATE
            UMI_AI_TEST_MANIFEST_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/manifest.txt")
    endforeach()
endif()

# Accumulated AdamW shares the canonical decoder's microbatch gradient reducer.
# Keep all previously registered experiments and Windows security tests intact.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-adamw-microbatch-parity-test
        framework.ai.training.adamw_microbatch_parity
        test_training_adamw_microbatch.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-adamw-microbatch-resume-test
        framework.ai.training.adamw_microbatch_resume
        test_training_adamw_microbatch_resume.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-adamw-microbatch-rejections-test
        framework.ai.training.adamw_microbatch_rejections
        test_training_adamw_microbatch_rejections.c)
    target_compile_definitions(umicom-ai-training-adamw-microbatch-resume-test PRIVATE
        UMI_AI_TEST_MANIFEST_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/manifest.txt")
endif()

# Review byte completions without altering decoder weights or checkpoint state.
# These tests do not claim C23 compilation or functional correctness.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-benchmark-score-test
        framework.ai.training.benchmark_score
        test_training_benchmark_score.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-benchmark-runner-test
        framework.ai.training.benchmark_runner
        test_training_benchmark_runner.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-benchmark-rejections-test
        framework.ai.training.benchmark_rejections
        test_training_benchmark_rejections.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-benchmark-reporting-test
        framework.ai.training.benchmark_reporting
        test_training_benchmark_reporting.c)
endif()

# Validate text-only reviewed completion suites and failure-atomic loading.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-benchmark-suite-test
        framework.ai.training.benchmark_suite
        test_training_benchmark_suite.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-benchmark-suite-rejections-test
        framework.ai.training.benchmark_suite_rejections
        test_training_benchmark_suite_rejections.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-benchmark-suite-file-test
        framework.ai.training.benchmark_suite_file
        test_training_benchmark_suite_file.c)
    target_compile_definitions(umicom-ai-training-benchmark-suite-file-test PRIVATE
        UMI_AI_TEST_BENCHMARK_SUITE_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/benchmark-reviewed.txt")
endif()

# Review SHA-256 identity, failure-atomic JSON publication and preserved
# benchmark semantics. Tests never run compiled model output or scripts.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-benchmark-evidence-identity-test
        framework.ai.training.benchmark_evidence_identity
        test_training_benchmark_evidence_identity.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-benchmark-evidence-json-test
        framework.ai.training.benchmark_evidence_json
        test_training_benchmark_evidence_json.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-benchmark-evidence-rejections-test
        framework.ai.training.benchmark_evidence_rejections
        test_training_benchmark_evidence_rejections.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-benchmark-evidence-changes-test
        framework.ai.training.benchmark_evidence_changes
        test_training_benchmark_evidence_changes.c)
endif()

# Native global L2 gradient clipping retains AdamW weight/moment atomicity.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-gradient-norm-test
        framework.ai.training.gradient_norm
        test_training_gradient_norm.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-gradient-clipping-rejections-test
        framework.ai.training.gradient_clipping_rejections
        test_training_gradient_clipping_rejections.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-adamw-clipping-parity-test
        framework.ai.training.adamw_clipping_parity
        test_training_adamw_clipping_parity.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-decoder-clipping-test
        framework.ai.training.decoder_clipping
        test_training_decoder_clipped.c)
endif()

# Explicitly reviewed gradient-stability policy remains opt-in. Legacy plan
# parsers and AdamW entry points still reject or ignore no extra settings.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-gradient-policy-plan-test
        framework.ai.training.gradient_policy_plan
        test_training_gradient_policy_plan.c)
    target_compile_definitions(umicom-ai-training-gradient-policy-plan-test PRIVATE
        UMI_AI_TEST_CLIPPED_PLAN_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/clipped-adamw-experiment.conf"
        UMI_AI_TEST_CLIPPED_MANIFEST_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/manifest.txt")
    _umicom_add_native_ai_training_test(
        umicom-ai-training-gradient-policy-identity-test
        framework.ai.training.gradient_policy_identity
        test_training_gradient_policy_identity.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-gradient-policy-file-test
        framework.ai.training.gradient_policy_file
        test_training_gradient_policy_file.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-adamw-clipped-experiment-resume-test
        framework.ai.training.adamw_clipped_experiment_resume
        test_training_adamw_clipped_experiment_resume.c)
    target_compile_definitions(umicom-ai-training-adamw-clipped-experiment-resume-test PRIVATE
        UMI_AI_TEST_MANIFEST_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/manifest.txt")
endif()

# The opt-in run lock protects exact checkpoint continuation against changed
# source catalogue order, optimiser hyperparameters, seeds or LR horizon.
# The on-disk regression exercises real checkpoint read/write and maths;
# it must NOT be replaced with mocks in the official Framework suite.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-run-lock-identity-test
        framework.ai.training.run_lock_identity
        test_training_run_lock_identity.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-run-lock-file-test
        framework.ai.training.run_lock_file
        test_training_run_lock_file.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-run-lock-disk-resume-test
        framework.ai.training.run_lock_disk_resume
        test_training_run_lock_disk_resume.c)
    foreach(_run_lock_target IN ITEMS
        umicom-ai-training-run-lock-identity-test
        umicom-ai-training-run-lock-file-test
        umicom-ai-training-run-lock-disk-resume-test)
        target_compile_definitions(${_run_lock_target} PRIVATE
            UMI_AI_TEST_MANIFEST_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/manifest.txt"
            UMI_AI_TEST_REORDERED_MANIFEST_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/manifest-reordered-for-run-lock.txt")
    endforeach()
endif()

# Opt-in stronger provenance adds cryptographic identity of actual imported
# source bytes without rewriting the previous run-lock/checkpoint formats.
# Exercise corpus vectors, manifest ordering and disk-companion rejection.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-corpus-source-sha256-test
        framework.ai.training.corpus_source_sha256
        test_training_corpus_source_sha256.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-dataset-source-sha256-test
        framework.ai.training.dataset_source_sha256
        test_training_dataset_source_sha256.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-source-lock-test
        framework.ai.training.source_lock
        test_training_source_lock.c)
    foreach(_source_identity_target IN ITEMS
        umicom-ai-training-dataset-source-sha256-test
        umicom-ai-training-source-lock-test)
        target_compile_definitions(${_source_identity_target} PRIVATE
            UMI_AI_TEST_MANIFEST_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/manifest.txt"
            UMI_AI_TEST_REORDERED_MANIFEST_PATH="${_umicom_ai_training_root}/learning/native-llm/fixtures/manifest-reordered-for-run-lock.txt")
    endforeach()
endif()

# A native, read-only publication verifier is useful even when the model
# training executable is quarantined. It does not bypass platform security,
# decode models, validate training semantics or execute generated text.
if(NOT TARGET umicom-native-llm-publication-verify)
    add_executable(umicom-native-llm-publication-verify
        "${_umicom_ai_training_root}/learning/native-llm/verify_training_publication.c")
    target_link_libraries(umicom-native-llm-publication-verify PRIVATE
        Umicom::AiTraining)
    target_compile_features(umicom-native-llm-publication-verify PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-native-llm-publication-verify PRIVATE /W4)
    else()
        target_compile_options(umicom-native-llm-publication-verify PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

# The tests exercise actual file publication, independent byte mutation,
# incomplete sidecars, interrupted staging and refusal to replace markers.
if(BUILD_TESTING)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-publication-file-test
        framework.ai.training.publication_file
        test_training_publication_file.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-publication-rejections-test
        framework.ai.training.publication_rejections
        test_training_publication_rejections.c)
endif()

# Optional cryptographic publisher attestation. The default native Framework
# remains entirely C23 and needs no additional cryptography dependencies.
# This explicitly enabled service uses OpenSSL's existing, reviewed Ed25519
# primitive; never implement new signature mathematics in an application.
# A signature binds the canonical, already-published checkpoint evidence to
# an independently supplied public-key trust anchor. It does NOT establish
# that a quarantined builder is safe, nor does it certify model correctness.
option(UMICOM_AI_TRAINING_ENABLE_SIGNATURES
    "Build opt-in native Ed25519 training publication signing and verification"
    OFF)
if(UMICOM_AI_TRAINING_ENABLE_SIGNATURES)
    find_package(OpenSSL 3.0 REQUIRED COMPONENTS Crypto)
    if(NOT TARGET umicom_ai_training_attestation)
        add_library(umicom_ai_training_attestation STATIC
            "${_umicom_ai_training_root}/src/ai/training/training_attestation.c")
        add_library(Umicom::AiTrainingAttestation ALIAS
            umicom_ai_training_attestation)
        target_include_directories(umicom_ai_training_attestation PUBLIC
            "$<BUILD_INTERFACE:${_umicom_ai_training_root}/include>")
        target_link_libraries(umicom_ai_training_attestation PUBLIC
            Umicom::AiTraining OpenSSL::Crypto)
        target_compile_features(umicom_ai_training_attestation PUBLIC c_std_23)
        if(MSVC)
            target_compile_options(umicom_ai_training_attestation PRIVATE /W4)
        else()
            target_compile_options(umicom_ai_training_attestation PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
    endif()

    if(NOT TARGET umicom-native-llm-publication-attest)
        add_executable(umicom-native-llm-publication-attest
            "${_umicom_ai_training_root}/learning/native-llm/attest_training_publication.c")
        target_link_libraries(umicom-native-llm-publication-attest PRIVATE
            Umicom::AiTrainingAttestation)
        target_compile_features(umicom-native-llm-publication-attest PRIVATE c_std_23)
        if(MSVC)
            target_compile_options(umicom-native-llm-publication-attest PRIVATE /W4)
        else()
            target_compile_options(umicom-native-llm-publication-attest PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
    endif()

    # Test private keys exist only in disposable local test workspaces. Tests
    # exercise real libcrypto and the real Framework publication verifier,
    # but never sign user models, ship keys or contact a remote authority.
    if(BUILD_TESTING)
        foreach(_umicom_attestation_case IN ITEMS roundtrip rejections)
            set(_umicom_attestation_target
                "umicom-ai-training-attestation-${_umicom_attestation_case}-test")
            if(NOT TARGET "${_umicom_attestation_target}")
                add_executable("${_umicom_attestation_target}"
                    "${_umicom_ai_training_root}/tests/ai_training/test_training_attestation_${_umicom_attestation_case}.c")
                target_link_libraries("${_umicom_attestation_target}" PRIVATE
                    Umicom::AiTrainingAttestation)
                target_compile_features("${_umicom_attestation_target}" PRIVATE c_std_23)
                if(MSVC)
                    target_compile_options("${_umicom_attestation_target}" PRIVATE /W4)
                else()
                    target_compile_options("${_umicom_attestation_target}" PRIVATE
                        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
                endif()
                add_test(
                    NAME "framework.ai.training.attestation_${_umicom_attestation_case}"
                    COMMAND "${_umicom_attestation_target}")
                set_tests_properties(
                    "framework.ai.training.attestation_${_umicom_attestation_case}"
                    PROPERTIES LABELS "framework;ai;native-training;cryptography")
            endif()
        endforeach()
    endif()
endif()

# Offline publisher trust: compose the existing Ed25519/publication verifier
# with a locally approved allowlist, explicit revocation, and bounded key
# validity intervals. An untrusted signature can choose only a candidate key
# already declared in the operator's independently controlled policy file.
# Keep this opt-in and do not introduce a second crypto implementation.
if(UMICOM_AI_TRAINING_ENABLE_SIGNATURES AND
   TARGET umicom_ai_training_attestation)
    target_sources(umicom_ai_training_attestation PRIVATE
        "${_umicom_ai_training_root}/src/ai/training/training_trust_policy.c")

    if(NOT TARGET umicom-native-llm-publication-trust)
        add_executable(umicom-native-llm-publication-trust
            "${_umicom_ai_training_root}/learning/native-llm/verify_training_trust.c")
        target_link_libraries(umicom-native-llm-publication-trust PRIVATE
            Umicom::AiTrainingAttestation)
        target_compile_features(umicom-native-llm-publication-trust PRIVATE c_std_23)
        if(MSVC)
            target_compile_options(umicom-native-llm-publication-trust PRIVATE /W4)
        else()
            target_compile_options(umicom-native-llm-publication-trust PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
    endif()

    if(BUILD_TESTING)
        foreach(_umicom_trust_case IN ITEMS parse files verify)
            set(_umicom_trust_target
                "umicom-ai-training-trust-policy-${_umicom_trust_case}-test")
            if(NOT TARGET "${_umicom_trust_target}")
                add_executable("${_umicom_trust_target}"
                    "${_umicom_ai_training_root}/tests/ai_training/test_training_trust_policy_${_umicom_trust_case}.c")
                target_link_libraries("${_umicom_trust_target}" PRIVATE
                    Umicom::AiTrainingAttestation)
                target_compile_features("${_umicom_trust_target}" PRIVATE c_std_23)
                if(MSVC)
                    target_compile_options("${_umicom_trust_target}" PRIVATE /W4)
                else()
                    target_compile_options("${_umicom_trust_target}" PRIVATE
                        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
                endif()
                add_test(
                    NAME "framework.ai.training.trust_policy_${_umicom_trust_case}"
                    COMMAND "${_umicom_trust_target}")
                set_tests_properties(
                    "framework.ai.training.trust_policy_${_umicom_trust_case}"
                    PROPERTIES LABELS "framework;ai;native-training;cryptography;trust")
            endif()
        endforeach()
    endif()
endif()

# Machine-readable offline verification receipts. This additive C23 module
# calls the existing trust-policy and Ed25519 verifier; it does not add a new
# signature algorithm, change checkpoint formats or enable crypto by default.
# It intentionally reuses the existing publication-trust command-line target
# rather than introducing another application's independent CLI parser.
if(UMICOM_AI_TRAINING_ENABLE_SIGNATURES AND
   TARGET umicom_ai_training_attestation)
    target_sources(umicom_ai_training_attestation PRIVATE
        "${_umicom_ai_training_root}/src/ai/training/training_trust_receipt.c")

    if(BUILD_TESTING)
        foreach(_umicom_receipt_case IN ITEMS roundtrip rejections)
            set(_umicom_receipt_target
                "umicom-ai-training-trust-receipt-${_umicom_receipt_case}-test")
            if(NOT TARGET "${_umicom_receipt_target}")
                add_executable("${_umicom_receipt_target}"
                    "${_umicom_ai_training_root}/tests/ai_training/test_training_trust_receipt_${_umicom_receipt_case}.c")
                target_link_libraries("${_umicom_receipt_target}" PRIVATE
                    Umicom::AiTrainingAttestation)
                target_compile_features("${_umicom_receipt_target}" PRIVATE c_std_23)
                if(MSVC)
                    target_compile_options("${_umicom_receipt_target}" PRIVATE /W4)
                else()
                    target_compile_options("${_umicom_receipt_target}" PRIVATE
                        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
                endif()
                add_test(
                    NAME "framework.ai.training.trust_receipt_${_umicom_receipt_case}"
                    COMMAND "${_umicom_receipt_target}")
                set_tests_properties(
                    "framework.ai.training.trust_receipt_${_umicom_receipt_case}"
                    PROPERTIES LABELS "framework;ai;native-training;cryptography;trust;receipt")
            endif()
        endforeach()
    endif()
endif()

# An ordered, all-or-nothing release gate for several independently signed
# native LLM publications. This optional Framework-owned C23 service calls
# existing publisher-policy and JSON receipt APIs; neither a second signature
# implementation nor a scripting-language release tool is introduced. Its
# aggregate digest is explicitly NOT a new signature or trusted timestamp.
if(UMICOM_AI_TRAINING_ENABLE_SIGNATURES AND
   TARGET umicom_ai_training_attestation)
    if(NOT TARGET umicom-native-llm-trust-batch)
        target_sources(umicom_ai_training_attestation PRIVATE
            "${_umicom_ai_training_root}/src/ai/training/training_trust_batch.c")

        add_executable(umicom-native-llm-trust-batch
            "${_umicom_ai_training_root}/learning/native-llm/verify_training_batch.c")
        target_link_libraries(umicom-native-llm-trust-batch PRIVATE
            Umicom::AiTrainingAttestation)
        target_compile_features(umicom-native-llm-trust-batch PRIVATE c_std_23)
        if(MSVC)
            target_compile_options(umicom-native-llm-trust-batch PRIVATE /W4)
        else()
            target_compile_options(umicom-native-llm-trust-batch PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
    endif()

    if(BUILD_TESTING)
        foreach(_umicom_batch_case IN ITEMS roundtrip rejections)
            set(_umicom_batch_target
                "umicom-ai-training-trust-batch-${_umicom_batch_case}-test")
            if(NOT TARGET "${_umicom_batch_target}")
                add_executable("${_umicom_batch_target}"
                    "${_umicom_ai_training_root}/tests/ai_training/test_training_trust_batch_${_umicom_batch_case}.c")
                target_link_libraries("${_umicom_batch_target}" PRIVATE
                    Umicom::AiTrainingAttestation)
                target_compile_features("${_umicom_batch_target}" PRIVATE c_std_23)
                if(MSVC)
                    target_compile_options("${_umicom_batch_target}" PRIVATE /W4)
                else()
                    target_compile_options("${_umicom_batch_target}" PRIVATE
                        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
                endif()
                add_test(
                    NAME "framework.ai.training.trust_batch_${_umicom_batch_case}"
                    COMMAND "${_umicom_batch_target}")
                set_tests_properties(
                    "framework.ai.training.trust_batch_${_umicom_batch_case}"
                    PROPERTIES LABELS "framework;ai;native-training;cryptography;trust;receipt;release-gate")
            endif()
        endforeach()
    endif()
endif()

# Independently approved, exact release identity. A reviewer pins the ordered
# SHA-256 of a previously verified batch outside its release-artifact directory.
# The new service re-runs the real Ed25519 trust-batch verifier and compares
# the reviewer's expected identity; it does not accept a release-supplied
# expectation as its own authority. Checkpoint formats and the original batch
# verifier are unchanged, and the dependency is optional as before.
if(UMICOM_AI_TRAINING_ENABLE_SIGNATURES AND
   TARGET umicom_ai_training_attestation)
    target_sources(umicom_ai_training_attestation PRIVATE
        "${_umicom_ai_training_root}/src/ai/training/training_release_approval.c")

    if(NOT TARGET umicom-native-llm-release-approve)
        add_executable(umicom-native-llm-release-approve
            "${_umicom_ai_training_root}/learning/native-llm/verify_training_release_approval.c")
        target_link_libraries(umicom-native-llm-release-approve PRIVATE
            Umicom::AiTrainingAttestation)
        target_compile_features(umicom-native-llm-release-approve PRIVATE c_std_23)
        if(MSVC)
            target_compile_options(umicom-native-llm-release-approve PRIVATE /W4)
        else()
            target_compile_options(umicom-native-llm-release-approve PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
    endif()

    if(BUILD_TESTING)
        foreach(_umicom_approval_case IN ITEMS parse file gate)
            set(_umicom_approval_target
                "umicom-ai-training-release-approval-${_umicom_approval_case}-test")
            if(NOT TARGET "${_umicom_approval_target}")
                add_executable("${_umicom_approval_target}"
                    "${_umicom_ai_training_root}/tests/ai_training/test_training_release_approval_${_umicom_approval_case}.c")
                target_link_libraries("${_umicom_approval_target}" PRIVATE
                    Umicom::AiTrainingAttestation)
                target_compile_features("${_umicom_approval_target}" PRIVATE c_std_23)
                if(MSVC)
                    target_compile_options("${_umicom_approval_target}" PRIVATE /W4)
                else()
                    target_compile_options("${_umicom_approval_target}" PRIVATE
                        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
                endif()
                add_test(
                    NAME "framework.ai.training.release_approval_${_umicom_approval_case}"
                    COMMAND "${_umicom_approval_target}")
                set_tests_properties(
                    "framework.ai.training.release_approval_${_umicom_approval_case}"
                    PROPERTIES LABELS "framework;ai;native-training;cryptography;trust;release-approval")
            endif()
        endforeach()
    endif()
endif()

# Platform-header symbol regression: compile the production attestation.c in
# a deliberately adversarial translation unit. A Win32 ReadFile prototype is
# introduced before the source: on Windows by windows.h itself, and on Unix
# by a compatible compile-only declaration. This prevents a previously
# Linux-invisible C symbol collision from reappearing during new work.
#
# The probe is an OBJECT library, never linked with the production library;
# it has no runtime side effects, no extra public API and no duplicate crypto
# definitions in binaries. Even a target-only signed Release build compiles
# this probe when the test configuration is enabled.
if(BUILD_TESTING AND UMICOM_AI_TRAINING_ENABLE_SIGNATURES AND
   TARGET umicom_ai_training_attestation AND
   NOT TARGET umicom_ai_training_attestation_win32_names_probe)
    add_library(umicom_ai_training_attestation_win32_names_probe OBJECT
        "${_umicom_ai_training_root}/tests/ai_training/test_training_attestation_win32_names.c")
    target_include_directories(umicom_ai_training_attestation_win32_names_probe PRIVATE
        "${_umicom_ai_training_root}/include")
    target_link_libraries(umicom_ai_training_attestation_win32_names_probe PRIVATE
        OpenSSL::Crypto)
    target_compile_features(umicom_ai_training_attestation_win32_names_probe
        PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_training_attestation_win32_names_probe
            PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_training_attestation_win32_names_probe
            PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    if(TARGET umicom-native-llm-release-approve)
        add_dependencies(umicom-native-llm-release-approve
            umicom_ai_training_attestation_win32_names_probe)
    endif()
endif()

# Independently signed release approval: a distinct, caller-approved Ed25519
# trust anchor authorises the exact existing plaintext approval. The original
# unsigned release-approval parser/CLI and batch verifier remain available;
# optional OpenSSL is the sole crypto dependency. No scripting runtime.
if(UMICOM_AI_TRAINING_ENABLE_SIGNATURES AND
   TARGET umicom_ai_training_attestation)
    target_sources(umicom_ai_training_attestation PRIVATE
        "${_umicom_ai_training_root}/src/ai/training/training_approval_attestation.c")

    if(NOT TARGET umicom-native-llm-signed-release)
        add_executable(umicom-native-llm-signed-release
            "${_umicom_ai_training_root}/learning/native-llm/verify_signed_release_approval.c")
        target_link_libraries(umicom-native-llm-signed-release PRIVATE
            Umicom::AiTrainingAttestation)
        target_compile_features(umicom-native-llm-signed-release PRIVATE c_std_23)
        if(MSVC)
            target_compile_options(umicom-native-llm-signed-release PRIVATE /W4)
        else()
            target_compile_options(umicom-native-llm-signed-release PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
    endif()

    if(BUILD_TESTING)
        foreach(_umicom_signed_case IN ITEMS roundtrip rejections)
            set(_umicom_signed_target
                "umicom-ai-training-signed-approval-${_umicom_signed_case}-test")
            if(NOT TARGET "${_umicom_signed_target}")
                add_executable("${_umicom_signed_target}"
                    "${_umicom_ai_training_root}/tests/ai_training/test_training_signed_approval_${_umicom_signed_case}.c")
                target_link_libraries("${_umicom_signed_target}" PRIVATE
                    Umicom::AiTrainingAttestation)
                target_compile_features("${_umicom_signed_target}" PRIVATE c_std_23)
                if(MSVC)
                    target_compile_options("${_umicom_signed_target}" PRIVATE /W4)
                else()
                    target_compile_options("${_umicom_signed_target}" PRIVATE
                        -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
                endif()
                add_test(
                    NAME "framework.ai.training.signed_approval_${_umicom_signed_case}"
                    COMMAND "${_umicom_signed_target}")
                set_tests_properties(
                    "framework.ai.training.signed_approval_${_umicom_signed_case}"
                    PROPERTIES LABELS "framework;ai;native-training;cryptography;trust;release-approval")
            endif()
        endforeach()
    endif()
endif()

# Read-only learner demonstration of the canonical decoder and KV-cache path.
# Register in the shared Framework composition so standalone/full builds use
# the same source owner and no application-local mathematical implementation.
if(TARGET umicom_ai_training AND NOT TARGET umicom-native-llm-kv-nucleus-demo)
    add_executable(umicom-native-llm-kv-nucleus-demo
        "${_umicom_ai_training_root}/learning/native-llm/inspect_generation_cache.c")
    target_link_libraries(umicom-native-llm-kv-nucleus-demo PRIVATE Umicom::AiTraining)
    target_compile_features(umicom-native-llm-kv-nucleus-demo PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-native-llm-kv-nucleus-demo PRIVATE /W4)
    else()
        target_compile_options(umicom-native-llm-kv-nucleus-demo PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

# A thin native teaching executable demonstrates streaming generation without
# duplicating decoder, sampling, or KV-cache implementation in an application.
if(TARGET umicom_ai_training AND NOT TARGET umicom-native-llm-streaming-demo)
    add_executable(umicom-native-llm-streaming-demo
        "${_umicom_ai_training_root}/learning/native-llm/inspect_generation_session.c")
    target_link_libraries(umicom-native-llm-streaming-demo PRIVATE Umicom::AiTraining)
    target_compile_features(umicom-native-llm-streaming-demo PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-native-llm-streaming-demo PRIVATE /W4)
    else()
        target_compile_options(umicom-native-llm-streaming-demo PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    if(BUILD_TESTING)
        add_test(NAME framework.ai.training.generation_session_demo
            COMMAND umicom-native-llm-streaming-demo)
        set_tests_properties(framework.ai.training.generation_session_demo PROPERTIES
            LABELS "framework;ai;native-training;learning;generation")
    endif()
endif()

# The learner CLI reports a measured reference-vs-cache comparison. Neither
# input source, model weights nor checkpoint bytes are changed; the native
# Framework owns both inference paths, all workload counters and resource sizes.
if(TARGET umicom_ai_training AND NOT TARGET umicom-native-llm-generation-work)
    add_executable(umicom-native-llm-generation-work
        "${_umicom_ai_training_root}/learning/native-llm/profile_generation_work.c")
    target_link_libraries(umicom-native-llm-generation-work PRIVATE Umicom::AiTraining)
    target_compile_features(umicom-native-llm-generation-work PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-native-llm-generation-work PRIVATE /W4)
    else()
        target_compile_options(umicom-native-llm-generation-work PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    if(BUILD_TESTING)
        add_test(NAME framework.ai.training.generation_performance_demo
            COMMAND umicom-native-llm-generation-work)
        set_tests_properties(framework.ai.training.generation_performance_demo PROPERTIES
            LABELS "framework;ai;native-training;learning;generation;performance")
    endif()
endif()
