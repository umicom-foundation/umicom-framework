#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/training_data/CMakeLists.txt
#
# PURPOSE:
#   Exercise bounded dataset input independently or in the Framework test graph.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomTrainingDataChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_data_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    # These are the production owners, not test doubles or another JSON parser.
    add_library(umicom_training_data_subject STATIC
        "${_data_root}/src/ai/training/jsonl_pairs.c"
        "${_data_root}/src/ai/training/jsonl_writer.c"
        "${_data_root}/src/ai/training/jsonl_summary.c"
        "${_data_root}/src/protocol/json_writer.c"
        "${_data_root}/src/base/utf8.c"
        "${_data_root}/src/language_runtime/json_tree.c"
        "${_data_root}/src/language_runtime/json_text.c"
        "${_data_root}/src/base/ascii_number.c"
        "${_data_root}/src/platform/cancellation.c"
        "${_data_root}/src/security/secrets.c")
    target_include_directories(umicom_training_data_subject PUBLIC "${_data_root}/include")
    set(_data_library umicom_training_data_subject)
    set_target_properties(umicom_training_data_subject PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(NOT MSVC)
        target_compile_options(umicom_training_data_subject PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
else()
    set(_data_library Umicom::ai)
endif()
foreach(_kind pairs scan writer summary partition)
    add_executable(umicom-training-jsonl-${_kind}-test "test_jsonl_${_kind}.c")
    target_link_libraries(umicom-training-jsonl-${_kind}-test PRIVATE ${_data_library})
    set_target_properties(umicom-training-jsonl-${_kind}-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(NOT MSVC)
        target_compile_options(umicom-training-jsonl-${_kind}-test PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-training-jsonl-${_kind}-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-training-jsonl-${_kind}-test)
    endif()
endforeach()
foreach(_case valid owned-copy unicode-escapes array null missing extra unknown wrong-type nested
    empty-source empty-completion duplicate escaped-duplicate decoded-nul surrogate invalid-utf8 trailing
    bom embedded-nul limits source-limit cancel arguments)
    add_test(NAME framework.training_jsonl.pairs.${_case}
        COMMAND umicom-training-jsonl-pairs-test ${_case})
    set_tests_properties(framework.training_jsonl.pairs.${_case}
        PROPERTIES TIMEOUT 20 LABELS "framework;training;dataset;ownership")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.training_jsonl.pairs.${_case} TARGETS umicom-training-jsonl-pairs-test)
    endif()
endforeach()
foreach(_case lf crlf last-line blank bad-second callback cancel-between cancel-before rows line-limit line-limit-crlf line-limit-final-cr line-limit-double-cr multiline arguments)
    add_test(NAME framework.training_jsonl.scan.${_case}
        COMMAND umicom-training-jsonl-scan-test ${_case})
    set_tests_properties(framework.training_jsonl.scan.${_case}
        PROPERTIES TIMEOUT 20 LABELS "framework;training;dataset;ownership")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.training_jsonl.scan.${_case} TARGETS umicom-training-jsonl-scan-test)
    endif()
endforeach()

get_filename_component(_example_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
add_executable(umicom-training-data-example-test "${_example_root}/examples/training_data/main.c")
target_link_libraries(umicom-training-data-example-test PRIVATE ${_data_library})
set_target_properties(umicom-training-data-example-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
add_test(NAME framework.training_jsonl.example COMMAND umicom-training-data-example-test)
set_tests_properties(framework.training_jsonl.example PROPERTIES TIMEOUT 20 LABELS "framework;ai;training;example")
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-training-data-example-test)
endif()
if(COMMAND umicom_test_impact_owners)
    umicom_test_impact_owners(framework.training_jsonl.example TARGETS umicom-training-data-example-test)
endif()

add_executable(umicom-training-jsonl-allocation-test test_jsonl_allocation.c)
target_link_libraries(umicom-training-jsonl-allocation-test PRIVATE ${_data_library})
set_target_properties(umicom-training-jsonl-allocation-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
add_test(NAME framework.training_jsonl.allocation COMMAND umicom-training-jsonl-allocation-test)
set_tests_properties(framework.training_jsonl.allocation PROPERTIES TIMEOUT 20 LABELS "framework;training;allocation;ownership")
if(COMMAND umicom_test_impact_owners)
    umicom_test_impact_owners(framework.training_jsonl.allocation TARGETS umicom-training-jsonl-allocation-test)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-training-jsonl-allocation-test)
endif()

foreach(_case roundtrip escapes empty-prompt exact-capacity late-utf8 unterminated empty-required expansion arguments)
    add_test(NAME framework.training_jsonl.writer.${_case}
        COMMAND umicom-training-jsonl-writer-test ${_case})
    set_tests_properties(framework.training_jsonl.writer.${_case}
        PROPERTIES TIMEOUT 20 LABELS "framework;training;dataset;ownership")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.training_jsonl.writer.${_case} TARGETS umicom-training-jsonl-writer-test)
    endif()
endforeach()

foreach(_case counts escaped-group unicode-bytes exact-identity reordered partial-refusal cancel rows unique-capacity long-id arguments)
    add_test(NAME framework.training_jsonl.summary.${_case}
        COMMAND umicom-training-jsonl-summary-test ${_case})
    set_tests_properties(framework.training_jsonl.summary.${_case}
        PROPERTIES TIMEOUT 20 LABELS "framework;training;dataset;ownership;summary")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.training_jsonl.summary.${_case} TARGETS umicom-training-jsonl-summary-test)
    endif()
endforeach()

# The file boundary and command use the real platform owners, including Unicode
# argv capture. A focused harness avoids building unrelated application targets.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    target_sources(umicom_training_data_subject PRIVATE
        "${_data_root}/src/ai/training/jsonl_file.c"
        "${_data_root}/src/platform/input_file.c"
        "${_data_root}/src/platform/output_file.c"
        "${_data_root}/src/platform/native_arguments.c"
        "${_data_root}/src/platform/native_arguments_capture.c")
    if(WIN32)
        target_link_libraries(umicom_training_data_subject PUBLIC shell32)
    endif()
endif()
add_executable(umicom-training-jsonl-file-test test_jsonl_file.c
    "${CMAKE_CURRENT_LIST_DIR}/../../tools/umicom/src/command_training_data.c")
target_link_libraries(umicom-training-jsonl-file-test PRIVATE ${_data_library})
target_compile_features(umicom-training-jsonl-file-test PRIVATE c_std_23)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-training-jsonl-file-test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-training-jsonl-file-test)
endif()
foreach(_case valid unicode crlf invalid-second blank duplicate empty oversize bom missing relative directory help extra unknown arguments cancel)
    add_test(NAME framework.training_jsonl.file.${_case}
        COMMAND "${CMAKE_COMMAND}"
            "-DSUBJECT=$<TARGET_FILE:umicom-training-jsonl-file-test>"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/file-cases/${_case}"
            "-DCASE=${_case}"
            -P "${CMAKE_CURRENT_LIST_DIR}/check_jsonl_file.cmake")
    set_tests_properties(framework.training_jsonl.file.${_case} PROPERTIES TIMEOUT 40 LABELS "framework;training;dataset;file;cli")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.training_jsonl.file.${_case} TARGETS umicom-training-jsonl-file-test)
    endif()
endforeach()

# The source partition adapter uses the production group splitter and RNG.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    target_sources(umicom_training_data_subject PRIVATE
        "${_data_root}/src/ai/training/jsonl_partition.c"
        "${_data_root}/src/ai/training/dataset_group_split.c"
        "${_data_root}/src/ai/training/initialisation.c")
    if(NOT WIN32)
        target_link_libraries(umicom_training_data_subject PUBLIC m)
    endif()
endif()
foreach(_case groups reordered repeated-rows escaped-id exact-id capacity partial-refusal group-count cancel row-limit arguments)
    add_test(NAME framework.training_jsonl.partition.${_case}
        COMMAND umicom-training-jsonl-partition-test ${_case})
    set_tests_properties(framework.training_jsonl.partition.${_case} PROPERTIES TIMEOUT 20 LABELS "framework;training;dataset;partition;ownership")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.training_jsonl.partition.${_case} TARGETS umicom-training-jsonl-partition-test)
    endif()
endforeach()

# Selection writes memory only and composes the production parser/writer.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    target_sources(umicom_training_data_subject PRIVATE "${_data_root}/src/ai/training/jsonl_selection.c")
endif()
add_executable(umicom-training-jsonl-selection-test test_jsonl_selection.c)
target_link_libraries(umicom-training-jsonl-selection-test PRIVATE ${_data_library})
target_compile_features(umicom-training-jsonl-selection-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-training-jsonl-selection-test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-training-jsonl-selection-test)
endif()
foreach(_case training evaluation all-and-none capacity membership-count invalid-unselected escaped-values cancel row-bound arguments)
    add_test(NAME framework.training_jsonl.selection.${_case} COMMAND umicom-training-jsonl-selection-test ${_case})
    set_tests_properties(framework.training_jsonl.selection.${_case} PROPERTIES TIMEOUT 20 LABELS "framework;training;dataset;selection;ownership")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.training_jsonl.selection.${_case} TARGETS umicom-training-jsonl-selection-test)
    endif()
endforeach()

# The installed-consumer source also demonstrates prediction counters.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    target_sources(umicom_training_data_subject PRIVATE "${_data_root}/src/ai/training/prediction_metrics.c")
endif()

# Exact pair comparison shares the parser and never trains or publishes a file.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    target_sources(umicom_training_data_subject PRIVATE "${_data_root}/src/ai/training/jsonl_overlap.c")
endif()
add_executable(umicom-training-jsonl-overlap-test test_jsonl_overlap.c)
target_link_libraries(umicom-training-jsonl-overlap-test PRIVATE ${_data_library})
set_target_properties(umicom-training-jsonl-overlap-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(NOT MSVC)
    target_compile_options(umicom-training-jsonl-overlap-test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-training-jsonl-overlap-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-training-jsonl-overlap-test)
endif()
foreach(_case distinct shared same-side escaped boundaries normalization budget membership partial cancel rows owned arguments)
    add_test(NAME framework.training_jsonl.overlap.${_case} COMMAND umicom-training-jsonl-overlap-test ${_case})
    set_tests_properties(framework.training_jsonl.overlap.${_case} PROPERTIES TIMEOUT 20 LABELS "framework;training;dataset;overlap;ownership")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.training_jsonl.overlap.${_case} TARGETS umicom-training-jsonl-overlap-test)
    endif()
endforeach()

# Context-fit counters reuse the production parser and geometry owner.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    target_sources(umicom_training_data_subject PRIVATE "${_data_root}/src/ai/training/byte_pair_shape.c"
        "${_data_root}/src/ai/training/jsonl_context.c")
endif()
add_executable(umicom-training-jsonl-context-test test_jsonl_context.c)
target_link_libraries(umicom-training-jsonl-context-test PRIVATE ${_data_library})
target_compile_features(umicom-training-jsonl-context-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-training-jsonl-context-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-training-jsonl-context-test)
endif()
foreach(_case IN ITEMS golden escapes exact all-overlong malformed cancel arguments bounds measure)
    add_test(NAME framework.training_jsonl.context.${_case} COMMAND umicom-training-jsonl-context-test ${_case})
    set_tests_properties(framework.training_jsonl.context.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;dataset;context;ownership")
endforeach()

# Compile the same example source against current production objects; installed
# package resolution remains a separate SDK qualification.
add_executable(umicom-training-context-example-test "${_example_root}/examples/training_data/context.c")
target_link_libraries(umicom-training-context-example-test PRIVATE ${_data_library})
target_compile_features(umicom-training-context-example-test PRIVATE c_std_23)
add_test(NAME framework.training_jsonl.context.example COMMAND umicom-training-context-example-test)
set_tests_properties(framework.training_jsonl.context.example PROPERTIES TIMEOUT 20 LABELS "framework;training;context;example")
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-training-context-example-test)
endif()

# Context file inspection shares the production file/CLI harness and has its own
# semantic cases; existing summary cases remain registered unchanged.
foreach(_case valid unicode crlf arguments cancel invalid-second missing relative directory oversize
    all-overlong all-fit zero above-limit negative plus space fraction overflow text hex leading-zero extra help)
    add_test(NAME framework.training_jsonl.context_file.${_case}
        COMMAND "${CMAKE_COMMAND}" "-DSUBJECT=$<TARGET_FILE:umicom-training-jsonl-file-test>"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/context-file-cases/${_case}" "-DCASE=${_case}"
            -P "${CMAKE_CURRENT_LIST_DIR}/check_jsonl_context_file.cmake")
    set_tests_properties(framework.training_jsonl.context_file.${_case}
        PROPERTIES TIMEOUT 40 LABELS "framework;training;dataset;file;cli;context")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.training_jsonl.context_file.${_case} TARGETS umicom-training-jsonl-file-test)
    endif()
endforeach()

# Exercise the wrapper itself with unavailable subjects, without compiling or
# launching substitute executables. A failed prerequisite must remain a failure.
foreach(_wrapper file context_file)
    foreach(_case missing directory)
        add_test(NAME framework.training_jsonl.subject_admission.${_wrapper}.${_case}
            COMMAND "${CMAKE_COMMAND}"
                "-DWRAPPER=${CMAKE_CURRENT_LIST_DIR}/check_jsonl_${_wrapper}.cmake"
                "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/subject-admission/${_wrapper}/${_case}"
                "-DCASE=${_case}" -P "${CMAKE_CURRENT_LIST_DIR}/check_subject_admission.cmake")
        set_tests_properties(framework.training_jsonl.subject_admission.${_wrapper}.${_case}
            PROPERTIES TIMEOUT 30 LABELS "framework;training;cmake;prerequisite")
    endforeach()
endforeach()

# File streaming reuses the native platform owner, not a buffered test double.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    target_sources(umicom_training_data_subject PRIVATE
        "${_data_root}/src/ai/training/jsonl_stream.c"
        "${_data_root}/src/platform/input_stream.c")
endif()
add_executable(umicom-training-jsonl-stream-test test_jsonl_stream.c)
target_link_libraries(umicom-training-jsonl-stream-test PRIVATE ${_data_library})
set_target_properties(umicom-training-jsonl-stream-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(NOT MSVC)
    target_compile_options(umicom-training-jsonl-stream-test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-training-jsonl-stream-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-training-jsonl-stream-test)
endif()
foreach(_case valid unicode crlf last-line final-cr empty blank late-invalid bom invalid-utf8 multiline
    line-max line-max-crlf line-max-cr line-over double-cr-over boundary-crlf boundary-utf8 boundary-escape
    rows-max rows-over oversize missing relative directory arguments cancel-before cancel-between cancel-final reject-second)
    add_test(NAME framework.training_jsonl.stream.${_case}
        COMMAND "${CMAKE_COMMAND}" "-DSUBJECT=$<TARGET_FILE:umicom-training-jsonl-stream-test>"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/stream-cases/${_case}" "-DCASE=${_case}"
            -P "${CMAKE_CURRENT_LIST_DIR}/check_jsonl_stream.cmake")
    set_tests_properties(framework.training_jsonl.stream.${_case} PROPERTIES
        TIMEOUT 40 LABELS "framework;training;dataset;stream;ownership")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.training_jsonl.stream.${_case} TARGETS umicom-training-jsonl-stream-test)
    endif()
endforeach()

# Observe only the stream owner's allocation; its parser and platform remain real.
add_executable(umicom-training-jsonl-stream-allocation-test test_jsonl_stream_allocation.c)
target_link_libraries(umicom-training-jsonl-stream-allocation-test PRIVATE ${_data_library})
set_target_properties(umicom-training-jsonl-stream-allocation-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(NOT MSVC)
    target_compile_options(umicom-training-jsonl-stream-allocation-test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-training-jsonl-stream-allocation-test)
endif()
foreach(_case valid late-invalid missing oversize)
    add_test(NAME framework.training_jsonl.stream_allocation.${_case}
        COMMAND "${CMAKE_COMMAND}" "-DSUBJECT=$<TARGET_FILE:umicom-training-jsonl-stream-test>"
            "-DALLOCATION_SUBJECT=$<TARGET_FILE:umicom-training-jsonl-stream-allocation-test>"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/stream-allocation-cases/${_case}" "-DCASE=${_case}"
            -P "${CMAKE_CURRENT_LIST_DIR}/check_jsonl_stream.cmake")
    set_tests_properties(framework.training_jsonl.stream_allocation.${_case} PROPERTIES
        TIMEOUT 40 LABELS "framework;training;dataset;stream;allocation;ownership")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.training_jsonl.stream_allocation.${_case}
            TARGETS umicom-training-jsonl-stream-test umicom-training-jsonl-stream-allocation-test)
    endif()
endforeach()

add_executable(umicom-training-stream-example-test "${_example_root}/examples/training_data/stream.c")
target_link_libraries(umicom-training-stream-example-test PRIVATE ${_data_library})
set_target_properties(umicom-training-stream-example-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(NOT MSVC)
    target_compile_options(umicom-training-stream-example-test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-training-stream-example-test)
endif()
foreach(_case valid late-invalid)
    add_test(NAME framework.training_jsonl.stream_example.${_case}
        COMMAND "${CMAKE_COMMAND}" "-DSUBJECT=$<TARGET_FILE:umicom-training-jsonl-stream-test>"
            "-DEXAMPLE_SUBJECT=$<TARGET_FILE:umicom-training-stream-example-test>"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/stream-example-cases/${_case}" "-DCASE=${_case}"
            -P "${CMAKE_CURRENT_LIST_DIR}/check_jsonl_stream.cmake")
    set_tests_properties(framework.training_jsonl.stream_example.${_case} PROPERTIES
        TIMEOUT 40 LABELS "framework;training;dataset;stream;example")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.training_jsonl.stream_example.${_case}
            TARGETS umicom-training-jsonl-stream-test umicom-training-stream-example-test)
    endif()
endforeach()


# One review shares canonical grouping/overlap; no second implementation exists.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    target_sources(umicom_training_data_subject PRIVATE "${_data_root}/src/ai/training/jsonl_review.c")
endif()
add_executable(umicom-training-jsonl-review-test test_jsonl_review.c)
target_link_libraries(umicom-training-jsonl-review-test PRIVATE ${_data_library})
target_compile_features(umicom-training-jsonl-review-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-training-jsonl-review-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-training-jsonl-review-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-training-jsonl-review-test)
endif()
foreach(_case shared seed-boundaries capacity budget budget-boundary late-malformed group-count cancel arguments source-bound)
    add_test(NAME framework.training_jsonl.review.${_case} COMMAND umicom-training-jsonl-review-test ${_case})
    set_tests_properties(framework.training_jsonl.review.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;dataset;review;ownership")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.training_jsonl.review.${_case} TARGETS umicom-training-jsonl-review-test)
    endif()
endforeach()

foreach(_case valid unicode crlf arguments cancel capacity missing relative directory oversize late-invalid budget groups distinct group-zero group-bound group-plus group-space seed-negative seed-overflow seed-hex seed-max seed-zero budget-zero budget-fraction budget-overflow budget-text leading-zero extra help)
    add_test(NAME framework.training_jsonl.review_file.${_case}
        COMMAND "${CMAKE_COMMAND}" "-DSUBJECT=$<TARGET_FILE:umicom-training-jsonl-file-test>"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/review-file-cases/${_case}" "-DCASE=${_case}"
            -P "${CMAKE_CURRENT_LIST_DIR}/check_jsonl_review_file.cmake")
    set_tests_properties(framework.training_jsonl.review_file.${_case} PROPERTIES TIMEOUT 50 LABELS "framework;training;dataset;review;file;cli")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.training_jsonl.review_file.${_case} TARGETS umicom-training-jsonl-file-test)
    endif()
endforeach()
