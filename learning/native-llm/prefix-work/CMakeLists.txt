#-----------------------------------------------------------------------------
# Umicom Framework — Native LLM Prefix-Reuse Laboratory
# File: learning/native-llm/prefix-work/CMakeLists.txt
# PURPOSE:
#   Opt-in isolated C23 project on top of the SAME Framework-owned training
#   and KV-cache modules. Existing native-LLM targets stay unchanged.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeLlmPrefixReuse LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
get_filename_component(_umicom_framework_root
    "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
include("${_umicom_framework_root}/cmake/UmicomNativeLlmTraining.cmake")
include("${_umicom_framework_root}/cmake/UmicomNativeLlmPrefixReuse.cmake")

# Paired canonical/full-context versus incremental-prefix benchmark.
# This additive module never rewrites the existing decoder, learned absolute
# positional embeddings, training state or checkpoint compatibility.
include("${_umicom_framework_root}/cmake/UmicomNativeLlmPrefixBenchmark.cmake")

# Bind the existing checkpoint to its declared learned-absolute positional
# strategy and explicitly selected generation context (no silent migration).
include("${_umicom_framework_root}/cmake/UmicomNativeLlmPositionContract.cmake")

# Enforce positional policy at the actual model-loading, generation, and
# benchmark boundaries. The 128-byte sidecar remains an additional record,
# never a modification of the established native decoder checkpoint format.
include("${_umicom_framework_root}/cmake/UmicomNativeLlmPositionAdmission.cmake")

# Opt-in RoPE research uses NEW model-family mathematics only. The existing
# learned-absolute decoder, checkpoint decoder and position admission service
# continue to reject incompatible policy or checkpoint reinterpretations.
include("${_umicom_framework_root}/cmake/UmicomNativeLlmRotaryResearch.cmake")

# Batch 83–84: prove the derivatives required for a future trainable RoPE
# decoder BEFORE making any change to the historic decoder or checkpoint ABI.
# The Framework-owned rotation implementation remains the sole RoPE owner.
include("${_umicom_framework_root}/cmake/UmicomNativeLlmRotaryTraining.cmake")

# Batch 85–86: bounded automatic differentiation over the existing Framework
# tape, using RoPE-derived matrices rather than changing tape implementation.
# The historical decoder and checkpoint formats remain completely untouched.
include("${_umicom_framework_root}/cmake/UmicomNativeLlmRotaryTape.cmake")

# Batch 87–88: a fully tape-owned training experiment with repeated SGD,
# independent final-loss evaluation, deterministic parameters and honest
# per-step convergence evidence. The original decoder and all checkpoints
# remain unchanged.
include("${_umicom_framework_root}/cmake/UmicomNativeLlmRotaryOptimisation.cmake")

# Batch 89–90: qualify previously implemented rotary-tape optimisation against
# an independent C23 causal-attention/MSE oracle and a numerical gradient check.
# When building this normal laboratory, these tests link the real Framework
# training tape; test-only numerical mathematics remains outside libraries.
include("${_umicom_framework_root}/cmake/UmicomNativeLlmRotaryQualification.cmake")

# Batch 91–92: additional production-target admission regressions for direct
# rotary-tape gradients, bounded allocation rollback and repeated graph rewind.
# None of the test-only code is linked into a deployed Framework application.
include("${_umicom_framework_root}/cmake/UmicomNativeLlmRotaryRealTape.cmake")

# Batch 93–94: interpret explicitly supplied CTest observations without
# inventing a successful production-tape run or altering the decoder.
include("${_umicom_framework_root}/cmake/UmicomNativeLlmRotaryAcceptance.cmake")
