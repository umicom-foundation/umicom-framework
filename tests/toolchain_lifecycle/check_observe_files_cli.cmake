#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/toolchain_lifecycle/check_observe_files_cli.cmake
#
# PURPOSE:
#   Verify the native selected-file command produces truthful JSON and preserves refused destinations.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

if(NOT DEFINED TOOL OR NOT DEFINED CASE)
    message(FATAL_ERROR "TOOL and CASE are required")
endif()
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _token)
set(_root "${CMAKE_CURRENT_BINARY_DIR}/file-observation-${CASE}-${_token}")
if(EXISTS "${_root}")
    message(FATAL_ERROR "Refuse an existing fixture directory")
endif()
file(MAKE_DIRECTORY "${_root}")
set(_source "${_root}/café source.bin")
set(_report "${_root}/report.json")
file(WRITE "${_source}" "abc")
set(_arguments --file "${_source}")
set(_expected 0)
if(CASE STREQUAL "stdout")
elseif(CASE STREQUAL "report")
    list(APPEND _arguments --report "${_report}")
elseif(CASE STREQUAL "existing")
    file(WRITE "${_report}" "existing evidence")
    list(APPEND _arguments --report "${_report}")
    set(_expected 1)
elseif(CASE STREQUAL "missing")
    list(APPEND _arguments --file "${_root}/missing.bin" --report "${_report}")
    set(_expected 1)
elseif(CASE STREQUAL "limit")
    list(APPEND _arguments --max-bytes 2 --report "${_report}")
    set(_expected 1)
elseif(CASE STREQUAL "source-destination")
    list(APPEND _arguments --report "${_source}")
    set(_expected 1)
else()
    message(FATAL_ERROR "Unknown fixture case")
endif()
execute_process(COMMAND "${TOOL}" observe-files ${_arguments}
    WORKING_DIRECTORY "${_root}" RESULT_VARIABLE _status OUTPUT_VARIABLE _stdout ERROR_VARIABLE _stderr TIMEOUT 15)
if(NOT "${_status}" STREQUAL "${_expected}")
    message(FATAL_ERROR "Unexpected exit ${_status}, expected ${_expected}: ${_stderr}")
endif()
file(READ "${_source}" _source_after)
if(NOT _source_after STREQUAL "abc")
    message(FATAL_ERROR "Source file changed")
endif()
if(CASE STREQUAL "stdout" OR CASE STREQUAL "report")
    if(NOT _stderr STREQUAL "")
        message(FATAL_ERROR "Unexpected diagnostic: ${_stderr}")
    endif()
    if(CASE STREQUAL "report")
        file(READ "${_report}" _json)
    else()
        set(_json "${_stdout}")
    endif()
    string(JSON _schema GET "${_json}" schema)
    string(JSON _count LENGTH "${_json}" files)
    string(JSON _path GET "${_json}" files 0 path)
    string(JSON _bytes GET "${_json}" files 0 bytes)
    string(JSON _hash GET "${_json}" files 0 sha256)
    string(JSON _atomic GET "${_json}" atomicSnapshot)
    string(JSON _loaded GET "${_json}" loadedModuleIdentity)
    string(JSON _qualified GET "${_json}" qualificationPassed)
    if(NOT _schema STREQUAL "umicom.file-observation" OR NOT _count EQUAL 1 OR
       NOT _path STREQUAL _source OR NOT _bytes EQUAL 3 OR _atomic OR _loaded OR _qualified OR
       NOT _hash STREQUAL "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")
        message(FATAL_ERROR "Incorrect observation contract: ${_json}")
    endif()
elseif(CASE STREQUAL "existing")
    file(READ "${_report}" _after)
    if(NOT _after STREQUAL "existing evidence")
        message(FATAL_ERROR "Old evidence changed")
    endif()
else()
    if(EXISTS "${_report}")
        message(FATAL_ERROR "Refused observation created a report")
    endif()
endif()
