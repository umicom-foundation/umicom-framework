#-----------------------------------------------------------------------------
# Umicom Framework
# File: learning/native-llm/check_dataset_inspector.cmake
#
# PURPOSE:
#   Check inspector exit codes, dataset identity and repeatable token reports.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(NOT DEFINED INSPECTOR OR NOT DEFINED MANIFEST)
    message(FATAL_ERROR "INSPECTOR and MANIFEST are required")
endif()
execute_process(COMMAND "${INSPECTOR}" "${MANIFEST}"
    RESULT_VARIABLE first_status OUTPUT_VARIABLE first_report
    ERROR_VARIABLE first_error TIMEOUT 10)
if(NOT first_status STREQUAL "0" OR NOT first_error STREQUAL "")
    message(FATAL_ERROR "First inspection failed: ${first_status}: ${first_error}")
endif()
if(NOT first_report MATCHES "Documents: 3; training: 2; evaluation: 1")
    message(FATAL_ERROR "Unexpected partition summary: ${first_report}")
endif()
string(REGEX MATCH "Dataset SHA-256: ([0-9a-f]+)" digest_line "${first_report}")
string(LENGTH "${CMAKE_MATCH_1}" digest_length)
if(NOT digest_length EQUAL 64)
    message(FATAL_ERROR "Missing complete dataset SHA-256")
endif()
if(NOT first_report MATCHES "Training input tokens: [0-9 ]+" OR
   NOT first_report MATCHES "Evaluation input tokens: [0-9 ]+")
    message(FATAL_ERROR "Missing sampled windows")
endif()
execute_process(COMMAND "${INSPECTOR}" "${MANIFEST}"
    RESULT_VARIABLE repeated_status OUTPUT_VARIABLE repeated_report
    ERROR_VARIABLE repeated_error TIMEOUT 10)
if(NOT repeated_status STREQUAL "0" OR NOT repeated_error STREQUAL "" OR
   NOT repeated_report STREQUAL first_report)
    message(FATAL_ERROR "An unchanged dataset did not produce an identical report")
endif()
# The opt-in report adds one identity line per already-reviewed source. Use
# CMake's independent file hashing to check the complete digest and preserve
# the default report byte for byte after removing exactly the added lines.
execute_process(COMMAND "${INSPECTOR}" --document-sha256 "${MANIFEST}"
    RESULT_VARIABLE document_status OUTPUT_VARIABLE document_report
    ERROR_VARIABLE document_error TIMEOUT 10)
if(NOT document_status STREQUAL "0" OR NOT document_error STREQUAL "")
    message(FATAL_ERROR "Document inspection failed: ${document_status}: ${document_error}")
endif()
get_filename_component(dataset_dir "${MANIFEST}" DIRECTORY)
foreach(source IN ITEMS training/project.txt training/document.txt evaluation/workspace.txt)
    file(SHA256 "${dataset_dir}/${source}" expected)
    string(FIND "${document_report}" "Document SHA-256: ${expected} ${source}\n" line)
    if(line LESS 0)
        message(FATAL_ERROR "Missing independently computed digest for ${source}")
    endif()
endforeach()
string(REGEX REPLACE "Document SHA-256: [0-9a-f]+ [^\n]+\n" "" without_documents "${document_report}")
if(NOT without_documents STREQUAL first_report OR first_report MATCHES "Document SHA-256:")
    message(FATAL_ERROR "Opt-in identities changed the default report")
endif()
execute_process(COMMAND "${INSPECTOR}" --unknown "${MANIFEST}"
    RESULT_VARIABLE unknown_status OUTPUT_VARIABLE unknown_report
    ERROR_VARIABLE unknown_error TIMEOUT 10)
if(NOT unknown_status STREQUAL "2" OR NOT unknown_report STREQUAL "" OR
   NOT unknown_error MATCHES "usage: umicom-native-llm-dataset")
    message(FATAL_ERROR "Unknown option did not produce the usage result")
endif()
execute_process(COMMAND "${INSPECTOR}"
    RESULT_VARIABLE usage_status OUTPUT_VARIABLE usage_report
    ERROR_VARIABLE usage_error TIMEOUT 10)
if(NOT usage_status STREQUAL "2" OR NOT usage_report STREQUAL "" OR
   NOT usage_error MATCHES "usage: umicom-native-llm-dataset")
    message(FATAL_ERROR "Missing arguments did not produce the usage result")
endif()
# A manifest path followed by a child cannot be a valid file: the known
# manifest is a regular file. This avoids relying on an absent global path.
execute_process(COMMAND "${INSPECTOR}" "${MANIFEST}/child"
    RESULT_VARIABLE rejected_status OUTPUT_VARIABLE rejected_report
    ERROR_VARIABLE rejected_error TIMEOUT 10)
if(NOT rejected_status STREQUAL "1" OR NOT rejected_report STREQUAL "" OR
   NOT rejected_error MATCHES "Dataset inspection failed")
    message(FATAL_ERROR "Unreadable manifest was not rejected cleanly")
endif()
