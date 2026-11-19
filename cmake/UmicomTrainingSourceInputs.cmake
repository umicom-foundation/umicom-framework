#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomTrainingSourceInputs.cmake
#
# PURPOSE:
#   Reject an incomplete native training source set before compilation begins.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Configuration owns file admission; no replacement, fetch or source generation
# occurs here. This checks completeness, not matching revisions or C validity.
# Aggregate missing paths so a partial merge can be repaired before a long build.
function(umicom_require_training_source_inputs root out_paths)
    if(NOT ARGC EQUAL 2 OR NOT IS_DIRECTORY "${root}" OR "${out_paths}" STREQUAL "")
        message(FATAL_ERROR "Training source admission requires a source directory and output variable.")
    endif()
    set(_required
        include/umicom/ai/training/jsonl_pairs.h
        include/umicom/ai/training/jsonl_summary.h
        include/umicom/ai/training/jsonl_file.h
        include/umicom/ai/training/jsonl_partition.h
        include/umicom/ai/training/dataset_split.h
        include/umicom/ai/training/initialisation.h
        include/umicom/ai/training/jsonl_selection.h
        include/umicom/ai/training/denoising.h
        src/ai/training/jsonl_pairs.c
        src/ai/training/jsonl_writer.c
        src/ai/training/jsonl_summary.c
        src/ai/training/jsonl_file.c
        src/ai/training/jsonl_partition.c
        src/ai/training/dataset_group_split.c
        src/ai/training/initialisation.c
        src/ai/training/jsonl_selection.c
        src/ai/training/denoising.c
        src/ai/training/normal_noise.c
        src/ai/training/noise_schedule.c
        # Row-order planning is part of the same training library; admit its
        # declaration and implementation together before a partial merge builds.
        include/umicom/ai/training/sample_order.h
        src/ai/training/sample_order.c
        include/umicom/ai/training/prediction_metrics.h
        src/ai/training/prediction_metrics.c
        # Graph loss and read-only evaluation share this required internal kernel.
        src/ai/training/categorical_loss.h
        include/umicom/ai/training/image_tensor.h
        src/ai/training/image_tensor.c
        include/umicom/ai/training/video_tensor.h
        src/ai/training/video_tensor.c
        include/umicom/ai/training/image_view.h
        src/ai/training/image_view.c
        include/umicom/ai/training/byte_pair_sequence.h
        src/ai/training/byte_pair_sequence.c
        include/umicom/ai/training/jsonl_overlap.h
        src/ai/training/jsonl_overlap.c
        include/umicom/ai/training/image_flip.h
        src/ai/training/image_flip.c
        include/umicom/ai/training/image_histogram.h
        src/ai/training/image_histogram.c
        include/umicom/ai/training/image_resize.h
        src/ai/training/image_resize.c
        include/umicom/ai/training/image_canvas.h
        src/ai/training/image_canvas.c
        src/ai/training/byte_pair_shape.c
        include/umicom/ai/training/jsonl_context.h
        src/ai/training/jsonl_context.c
        include/umicom/ai/training/video_timing.h
        src/ai/training/video_timing.c
        include/umicom/ai/training/jsonl_stream.h
        src/ai/training/jsonl_stream.c
        include/umicom/ai/training/jsonl_review.h
        src/ai/training/jsonl_review.c
        include/umicom/ai/training/image_moments.h
        src/ai/training/image_moments.c
    )
    set(_paths)
    set(_unavailable)
    foreach(_relative IN LISTS _required)
        get_filename_component(_path "${root}/${_relative}" ABSOLUTE)
        if(NOT EXISTS "${_path}" OR IS_DIRECTORY "${_path}")
            list(APPEND _unavailable "${_relative} (missing or not a file)")
        else()
            file(SIZE "${_path}" _bytes)
            if(_bytes EQUAL 0)
                list(APPEND _unavailable "${_relative} (empty file)")
            endif()
        endif()
        list(APPEND _paths "${_path}")
    endforeach()
    if(_unavailable)
        list(JOIN _unavailable "
  " _details)
        message(FATAL_ERROR
            "Native training source set is incomplete:
  ${_details}
"
            "Integrate the complete matching Framework headers and sources, then configure again. "
            "Do not substitute empty headers or continue qualification with stale executables.")
    endif()
    set("${out_paths}" "${_paths}" PARENT_SCOPE)
endfunction()
