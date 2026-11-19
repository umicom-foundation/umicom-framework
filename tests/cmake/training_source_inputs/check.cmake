#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/cmake/training_source_inputs/check.cmake
#
# PURPOSE:
#   Exercise complete, missing, empty and non-file training inputs without mutating the source checkout.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(NOT DEFINED MODULE OR NOT EXISTS "${MODULE}")
    message(FATAL_ERROR "MODULE must name the source admission implementation")
endif()
if(DEFINED PROBE_ROOT)
    include("${MODULE}")
    umicom_require_training_source_inputs("${PROBE_ROOT}" _admitted)
    list(LENGTH _admitted _count)
    # The previous inventory is retained for review; crop header/source add two inputs.
    if(FALSE)
    if(NOT _count EQUAL 23)
        message(FATAL_ERROR "Unexpected admitted input count: ${_count}")
    endif()
    endif()
    # Preserve the earlier crop-only count; byte-pair preparation adds two inputs.
    if(FALSE)
    if(NOT _count EQUAL 25)
        message(FATAL_ERROR "Unexpected admitted input count: ${_count}")
    endif()
    endif()
    # Keep the byte-pair-only inventory check for review; overlap adds two inputs.
    if(FALSE)
    if(NOT _count EQUAL 27)
        message(FATAL_ERROR "Unexpected admitted input count: ${_count}")
    endif()
    endif()
    # Retain overlap-only count guard; orientation adds two admitted inputs.
    if(FALSE)
    if(NOT _count EQUAL 29)
        message(FATAL_ERROR "Unexpected admitted input count: ${_count}")
    endif()
    endif()
    # Preserve the orientation-only count; histogram adds its header and source.
    if(FALSE)
    if(NOT _count EQUAL 31)
        message(FATAL_ERROR "Unexpected admitted input count: ${_count}")
    endif()
    endif()
    # Preserve histogram-only inventory while requiring the complete resize set.
    if(FALSE)
    if(NOT _count EQUAL 33)
        message(FATAL_ERROR "Unexpected admitted input count: ${_count}")
    endif()
    endif()
    # Preserve the resize-only count; canvas adds two source inputs.
    if(FALSE)
    if(NOT _count EQUAL 35)
        message(FATAL_ERROR "Unexpected admitted input count: ${_count}")
    endif()
    endif()
    # Preserve canvas-only inventory; context inspection adds three inputs.
    if(FALSE)
    if(NOT _count EQUAL 37)
        message(FATAL_ERROR "Unexpected admitted input count: ${_count}")
    endif()
    endif()
    # Retain context-only inventory guard; explicit timing adds two inputs.
    if(FALSE)
    if(NOT _count EQUAL 40)
        message(FATAL_ERROR "Unexpected admitted input count: ${_count}")
    endif()
    endif()
    # Preserve timing-only inventory; the stream visitor adds two inputs.
    if(FALSE)
    if(NOT _count EQUAL 42)
        message(FATAL_ERROR "Unexpected admitted input count: ${_count}")
    endif()
    endif()
    # Preserve stream-only inventory; atomic review adds declaration and source.
    if(FALSE)
    if(NOT _count EQUAL 44)
        message(FATAL_ERROR "Unexpected admitted input count: ${_count}")
    endif()
    endif()
    # Preserve review-only inventory; image moments add two inputs.
    if(FALSE)
    if(NOT _count EQUAL 46)
        message(FATAL_ERROR "Unexpected admitted input count: ${_count}")
    endif()
    endif()
    # Preserve the image-moments inventory; categorical loss adds one internal header.
    if(FALSE)
    if(NOT _count EQUAL 48)
        message(FATAL_ERROR "Unexpected admitted input count: ${_count}")
    endif()
    endif()
    # Preserve the fixed pre-partition inventory for review. The probe now
    # compares against the parent fixture's independent inventory size.
    if(FALSE)
    if(NOT _count EQUAL 49)
        message(FATAL_ERROR "Unexpected admitted input count: ${_count}")
    endif()
    endif()
    if(NOT DEFINED PROBE_EXPECTED_COUNT OR NOT _count EQUAL PROBE_EXPECTED_COUNT)
        message(FATAL_ERROR "Unexpected admitted input count: ${_count}; expected ${PROBE_EXPECTED_COUNT}")
    endif()
    foreach(_path IN LISTS _admitted)
        if(NOT IS_ABSOLUTE "${_path}")
            message(FATAL_ERROR "An admitted input was not absolute")
        endif()
    endforeach()
    # Previous report: Admitted 23 training source inputs, before crop integration.
    # Earlier report retained: Admitted 25 training source inputs.
    # Earlier report retained: Admitted 27 training source inputs.
    # Earlier report retained: Admitted 29 training source inputs.
    # Earlier report retained: Admitted 31 training source inputs.
    # Previous report retained: Admitted 33 training source inputs.
    # Previous report retained: Admitted 35 training source inputs.
    # Previous report retained: Admitted 37 training source inputs.
    # Prior receipt retained: Admitted 40 training source inputs.
    # Prior receipt retained: Admitted 42 training source inputs.
    # Prior receipt retained: Admitted 44 training source inputs.
    # Prior receipt retained: Admitted 46 training source inputs.
    # Prior receipt retained: Admitted 48 training source inputs.
    # Prior receipt retained: Admitted 49 training source inputs.
    message(STATUS "Admitted ${_count} training source inputs")
    return()
endif()
if(NOT DEFINED CASE OR NOT DEFINED WORK)
    message(FATAL_ERROR "CASE and WORK are required")
endif()
# An independent fixture inventory makes accidentally dropping a required
# header from the implementation observable in that missing-header case.
set(_inputs
    include/umicom/ai/training/jsonl_pairs.h
    include/umicom/ai/training/jsonl_summary.h
    include/umicom/ai/training/jsonl_file.h
    include/umicom/ai/training/jsonl_partition.h
    include/umicom/ai/training/jsonl_selection.h
    include/umicom/ai/training/denoising.h
    src/ai/training/jsonl_pairs.c
    src/ai/training/jsonl_writer.c
    src/ai/training/jsonl_summary.c
    src/ai/training/jsonl_file.c
    src/ai/training/jsonl_partition.c
    src/ai/training/jsonl_selection.c
    src/ai/training/denoising.c
    src/ai/training/normal_noise.c
    src/ai/training/noise_schedule.c
    include/umicom/ai/training/sample_order.h
    src/ai/training/sample_order.c
    include/umicom/ai/training/prediction_metrics.h
    src/ai/training/prediction_metrics.c
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
    src/ai/training/categorical_loss.h
    include/umicom/ai/training/dataset_split.h
    include/umicom/ai/training/initialisation.h
    src/ai/training/dataset_group_split.c
    src/ai/training/initialisation.c)
set(_missing_cases missing-pairs-header missing-summary-header missing-file-header
    missing-partition-header missing-selection-header missing-denoising-header
    missing-pairs-source missing-writer-source missing-summary-source missing-file-source
    missing-partition-source missing-selection-source missing-denoising-source
    missing-normal-source missing-schedule-source missing-order-header missing-order-source missing-metrics-header missing-metrics-source missing-image-header missing-image-source missing-video-header missing-video-source missing-view-header missing-view-source missing-byte-pair-header missing-byte-pair-source missing-overlap-header missing-overlap-source missing-flip-header missing-flip-source missing-histogram-header missing-histogram-source missing-resize-header missing-resize-source missing-canvas-header missing-canvas-source missing-shape-source missing-context-header missing-context-source missing-timing-header missing-timing-source missing-stream-header missing-stream-source missing-review-header missing-review-source missing-moments-header missing-moments-source missing-categorical-kernel)
list(APPEND _missing_cases missing-group-header missing-seed-header missing-group-source missing-seed-source)
list(FIND _missing_cases "${CASE}" _missing_index)
set(_allowed valid empty-header directory-header multiple-missing missing-root ${_missing_cases})
if(NOT CASE IN_LIST _allowed)
    message(FATAL_ERROR "Unknown source-input fixture case: ${CASE}")
endif()
string(RANDOM LENGTH 20 ALPHABET 0123456789abcdef _suffix)
set(_root "${WORK}/source with spaces-${_suffix}")
set(_expected)
set(_written)
if(NOT CASE STREQUAL "missing-root")
    # Retain the smaller fixture for review; the active fixture covers crop inputs too.
    if(FALSE)
    foreach(_index RANGE 0 22)
        list(GET _inputs ${_index} _relative)
        set(_path "${_root}/${_relative}")
        if(_index EQUAL _missing_index OR
            (CASE STREQUAL "multiple-missing" AND (_index EQUAL 0 OR _index EQUAL 5 OR _index EQUAL 14 OR _index EQUAL 16 OR _index EQUAL 18 OR _index EQUAL 20 OR _index EQUAL 22)))
            list(APPEND _expected "${_relative}")
            continue()
        endif()
        get_filename_component(_parent "${_path}" DIRECTORY)
        file(MAKE_DIRECTORY "${_parent}")
        if(_index EQUAL 0 AND CASE STREQUAL "directory-header")
            file(MAKE_DIRECTORY "${_path}")
            list(APPEND _expected "${_relative}")
        elseif(_index EQUAL 0 AND CASE STREQUAL "empty-header")
            file(WRITE "${_path}" "")
            list(APPEND _expected "${_relative}")
            list(APPEND _written "${_path}")
        else()
            file(WRITE "${_path}" "/* Presence fixture; deliberately not compilation evidence. */
")
            list(APPEND _written "${_path}")
        endif()
    endforeach()
    endif()
    # Keep the earlier 25-input fixture for review; iterate the complete inventory.
    if(FALSE)
    foreach(_index RANGE 0 24)
        list(GET _inputs ${_index} _relative)
        set(_path "${_root}/${_relative}")
        if(_index EQUAL _missing_index OR
            (CASE STREQUAL "multiple-missing" AND (_index EQUAL 0 OR _index EQUAL 5 OR _index EQUAL 14 OR _index EQUAL 16 OR _index EQUAL 18 OR _index EQUAL 20 OR _index EQUAL 22 OR _index EQUAL 24)))
            list(APPEND _expected "${_relative}")
            continue()
        endif()
        get_filename_component(_parent "${_path}" DIRECTORY)
        file(MAKE_DIRECTORY "${_parent}")
        if(_index EQUAL 0 AND CASE STREQUAL "directory-header")
            file(MAKE_DIRECTORY "${_path}")
            list(APPEND _expected "${_relative}")
        elseif(_index EQUAL 0 AND CASE STREQUAL "empty-header")
            file(WRITE "${_path}" "")
            list(APPEND _expected "${_relative}")
            list(APPEND _written "${_path}")
        else()
            file(WRITE "${_path}" "/* Presence fixture; deliberately not compilation evidence. */
")
            list(APPEND _written "${_path}")
        endif()
    endforeach()
    endif()
    set(_index -1)
    foreach(_relative IN LISTS _inputs)
        math(EXPR _index "${_index} + 1")
        set(_path "${_root}/${_relative}")
        if(_index EQUAL _missing_index OR
            (CASE STREQUAL "multiple-missing" AND (_index EQUAL 0 OR _index EQUAL 5 OR _index EQUAL 14 OR _index EQUAL 16 OR _index EQUAL 18 OR _index EQUAL 20 OR _index EQUAL 22 OR _index EQUAL 24 OR _index EQUAL 26)))
            list(APPEND _expected "${_relative}")
            continue()
        endif()
        get_filename_component(_parent "${_path}" DIRECTORY)
        file(MAKE_DIRECTORY "${_parent}")
        if(_index EQUAL 0 AND CASE STREQUAL "directory-header")
            file(MAKE_DIRECTORY "${_path}")
            list(APPEND _expected "${_relative}")
        elseif(_index EQUAL 0 AND CASE STREQUAL "empty-header")
            file(WRITE "${_path}" "")
            list(APPEND _expected "${_relative}")
            list(APPEND _written "${_path}")
        else()
            file(WRITE "${_path}" "/* Presence fixture; deliberately not compilation evidence. */
")
            list(APPEND _written "${_path}")
        endif()
    endforeach()
endif()
set(_digests)
foreach(_path IN LISTS _written)
    file(SHA256 "${_path}" _digest)
    list(APPEND _digests "${_digest}")
endforeach()
list(LENGTH _inputs _expected_count)
execute_process(COMMAND "${CMAKE_COMMAND}" "-DMODULE=${MODULE}" "-DPROBE_ROOT=${_root}"
    "-DPROBE_EXPECTED_COUNT=${_expected_count}"
    -P "${CMAKE_CURRENT_LIST_FILE}" RESULT_VARIABLE _code OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 10)
if(CASE STREQUAL "valid")
    # Previous successful receipt expected 23 inputs; the complete crop set has 25.
    # Preserve the crop-only receipt guard while requiring the complete new inventory.
    if(FALSE)
    if(NOT "${_code}" STREQUAL "0" OR NOT _out MATCHES "Admitted 25 training source inputs")
        message(FATAL_ERROR "Complete source set refused: ${_code}
${_out}
${_err}")
    endif()
    endif()
    # Preserve the earlier success receipt while checking the expanded inventory.
    if(FALSE)
    if(NOT "${_code}" STREQUAL "0" OR NOT _out MATCHES "Admitted 27 training source inputs")
        message(FATAL_ERROR "Complete source set refused: ${_code}
${_out}
${_err}")
    endif()
    endif()
    # Preserve earlier receipt while requiring the expanded input inventory.
    if(FALSE)
    if(NOT "${_code}" STREQUAL "0" OR NOT _out MATCHES "Admitted 29 training source inputs")
        message(FATAL_ERROR "Complete source set refused: ${_code}
${_out}
${_err}")
    endif()
    endif()
    # Keep the earlier receipt for review; require the complete histogram set.
    if(FALSE)
    if(NOT "${_code}" STREQUAL "0" OR NOT _out MATCHES "Admitted 31 training source inputs")
        message(FATAL_ERROR "Complete source set refused: ${_code}
${_out}
${_err}")
    endif()
    endif()
    # Retain earlier receipt; source admission now includes the resize contract.
    if(FALSE)
    if(NOT "${_code}" STREQUAL "0" OR NOT _out MATCHES "Admitted 33 training source inputs")
        message(FATAL_ERROR "Complete source set refused: ${_code}
${_out}
${_err}")
    endif()

    endif()
    # Retain prior receipt; require the canvas input inventory.
    if(FALSE)
    if(NOT "${_code}" STREQUAL "0" OR NOT _out MATCHES "Admitted 35 training source inputs")
        message(FATAL_ERROR "Complete source set refused: ${_code}
${_out}
${_err}")
    endif()
    endif()
    # Retain the former message assertion; derive display count from this
    # independent fixture inventory while the child keeps its exact count guard.
    if(FALSE)
    if(NOT "${_code}" STREQUAL "0" OR NOT _out MATCHES "Admitted 37 training source inputs")
        message(FATAL_ERROR "Complete source set refused: ${_code}
${_out}
${_err}")
    endif()
    endif()
    if(NOT "${_code}" STREQUAL "0" OR NOT _out MATCHES "Admitted ${_expected_count} training source inputs")
        message(FATAL_ERROR "Complete source set refused: ${_code}\n${_out}\n${_err}")
    endif()
else()
    if("${_code}" STREQUAL "0")
        message(FATAL_ERROR "Incomplete source set was accepted: ${CASE}")
    endif()
    if(CASE STREQUAL "missing-root")
        set(_signature "Training source admission requires a source directory")
    else()
        set(_signature "Native training source set is incomplete")
    endif()
    string(FIND "${_err}" "${_signature}" _found)
    if(_found LESS 0)
        message(FATAL_ERROR "Unexpected refusal: ${_err}")
    endif()
    foreach(_relative IN LISTS _expected)
        string(FIND "${_err}" "${_relative}" _found)
        if(_found LESS 0)
            message(FATAL_ERROR "Missing path omitted from aggregate diagnostic: ${_relative}
${_err}")
        endif()
    endforeach()
    if(CASE STREQUAL "empty-header" AND NOT _err MATCHES "empty file")
        message(FATAL_ERROR "Empty-header reason was lost")
    endif()
endif()
# The admission helper is read-only even when it refuses the source set.
set(_index 0)
foreach(_path IN LISTS _written)
    file(SHA256 "${_path}" _after)
    list(GET _digests ${_index} _before)
    if(NOT _after STREQUAL _before)
        message(FATAL_ERROR "Source admission modified an input")
    endif()
    math(EXPR _index "${_index} + 1")
endforeach()
