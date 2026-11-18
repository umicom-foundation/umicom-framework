#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomTrainingData.cmake
#
# PURPOSE:
#   Share bounded teaching-data readers through the existing AI SDK library.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
set(_umicom_training_data_root "${CMAKE_CURRENT_LIST_DIR}/..")

# Public headers and their source owners are one integration unit. A missing
# header must fail configuration, not hundreds of minutes into compilation.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomTrainingSourceInputs.cmake")
umicom_require_training_source_inputs("${_umicom_training_data_root}" _umicom_training_inputs)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${_umicom_training_inputs})
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/jsonl_pairs.c"
    "${_umicom_training_data_root}/src/ai/training/jsonl_writer.c"
    "${_umicom_training_data_root}/src/ai/training/jsonl_summary.c")
# The JSON implementation already belongs to Protocol. Reuse its ownership,
# Unicode and duplicate-key rules rather than adding an AI-specific parser.
target_link_libraries(umicom_ai PRIVATE Umicom::protocol Umicom::security)
if(BUILD_TESTING)
    add_subdirectory("${_umicom_training_data_root}/tests/training_data"
        "${CMAKE_CURRENT_BINARY_DIR}/training-data-checks")
endif()

foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES "${_umicom_training_data_root}/examples/training_data/main.c"
        "${_umicom_training_data_root}/examples/training_data/context.c"
        "${_umicom_training_data_root}/examples/training_data/stream.c"
        "${_umicom_training_data_root}/examples/training_data/CMakeLists.txt"
        "${_umicom_training_data_root}/examples/training_data/README.md"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/training_data"
        COMPONENT "${_component}" ${_selection})
endforeach()

# Denoising arithmetic is a shared native AI operation; applications supply
# their model-specific normalization, coefficient schedule and noise source.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/denoising.c")
if(NOT WIN32)
    target_link_libraries(umicom_ai PRIVATE m)
endif()
if(BUILD_TESTING)
    add_subdirectory("${_umicom_training_data_root}/tests/denoising"
        "${CMAKE_CURRENT_BINARY_DIR}/denoising-checks")
endif()

# Native clients share file admission and statistics; the command is presentation.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/jsonl_file.c")
target_link_libraries(umicom_ai PRIVATE Umicom::platform)
if(TARGET umicom)
    target_sources(umicom PRIVATE "${_umicom_training_data_root}/tools/umicom/src/command_training_data.c")
    target_link_libraries(umicom PRIVATE Umicom::ai)
endif()

# Applications reuse exact-source partitioning instead of hashing source names.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/jsonl_partition.c")

# Keep transcendental normal sampling outside the existing integer RNG owner.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/normal_noise.c")

# The cumulative-noise conversion shares the denoising public contract.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/noise_schedule.c")

# Reviewed membership output reuses the same JSONL parser/writer contracts.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/jsonl_selection.c")

if(BUILD_TESTING)
    add_subdirectory("${_umicom_training_data_root}/tests/cmake/training_source_inputs"
        "${CMAKE_CURRENT_BINARY_DIR}/training-source-input-checks")
endif()

# Evaluation counters are part of the reusable AI SDK as well as the teaching
# trainer. Both targets compile the same allocation-free implementation.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/prediction_metrics.c")
if(BUILD_TESTING)
    add_subdirectory("${_umicom_training_data_root}/tests/training_metrics"
        "${CMAKE_CURRENT_BINARY_DIR}/prediction-metrics-checks")
endif()

# Pixel-to-model adaptation is explicit and shared by native applications.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/image_tensor.c")
if(BUILD_TESTING)
    add_subdirectory("${_umicom_training_data_root}/tests/image_tensor"
        "${CMAKE_CURRENT_BINARY_DIR}/image-tensor-checks")
endif()

# Temporal training inputs reuse the same image admission and normalization.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/video_tensor.c")
if(BUILD_TESTING)
    add_subdirectory("${_umicom_training_data_root}/tests/video_tensor"
        "${CMAKE_CURRENT_BINARY_DIR}/video-tensor-checks")
endif()

# Cropping borrows checked pixel spans; it shares the same image shape owner.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/image_view.c")
if(BUILD_TESTING)
    add_subdirectory("${_umicom_training_data_root}/tests/image_view"
        "${CMAKE_CURRENT_BINARY_DIR}/image-view-checks")
endif()

# Dataset overlap inspection reuses the shared JSON owner and belongs to ai.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/jsonl_overlap.c")

# Explicit orientation reuses the shared RGB shape owner for application clients.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/image_flip.c")
if(BUILD_TESTING AND NOT TARGET umicom-image-flip-test)
    add_subdirectory("${_umicom_training_data_root}/tests/image_flip"
        "${CMAKE_CURRENT_BINARY_DIR}/image-flip-checks")
endif()

# Discrete decoded-pixel measurements share the same bounded RGB view admission.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/image_histogram.c")
if(BUILD_TESTING AND NOT TARGET umicom-image-histogram-test)
    add_subdirectory("${_umicom_training_data_root}/tests/image_histogram"
        "${CMAKE_CURRENT_BINARY_DIR}/image-histogram-checks")
endif()

# Explicit pixel-center resampling belongs to the shared bounded training adapter.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/image_resize.c")
if(BUILD_TESTING AND NOT TARGET umicom-image-resize-test)
    add_subdirectory("${_umicom_training_data_root}/tests/image_resize"
        "${CMAKE_CURRENT_BINARY_DIR}/image-resize-checks")
endif()

# Canvas placement preserves source pixels and makes border values explicit.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/image_canvas.c")
if(BUILD_TESTING AND NOT TARGET umicom-image-canvas-test)
    add_subdirectory("${_umicom_training_data_root}/tests/image_canvas"
        "${CMAKE_CURRENT_BINARY_DIR}/image-canvas-checks")
endif()

# Context inspection measures exact native byte rows without building token arrays.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/byte_pair_shape.c"
    "${_umicom_training_data_root}/src/ai/training/jsonl_context.c")

# Timestamp policy is shared by exported AI and native teaching clients.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/video_timing.c")
if(BUILD_TESTING AND NOT TARGET umicom-video-timing-test)
    add_subdirectory("${_umicom_training_data_root}/tests/video_timing"
        "${CMAKE_CURRENT_BINARY_DIR}/video-timing-checks")
endif()

# Reuse platform streaming for bounded record retention in native clients.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/jsonl_stream.c")

# Atomic review composes the canonical splitter and overlap owners in the SDK.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/jsonl_review.c")

# Channel moments derive from the same admitted histogram implementation.
target_sources(umicom_ai PRIVATE "${_umicom_training_data_root}/src/ai/training/image_moments.c")
