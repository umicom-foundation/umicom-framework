#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/test_reuse/check_receipt_file.cmake
#
# PURPOSE:
#   Exercise native receipt files with explicit portable and Windows newlines.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.29)
foreach(_required PROGRAM WORK CASE)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "Supply PROGRAM, WORK and CASE")
    endif()
endforeach()
set(_cases lf crlf empty bare-cr duplicate unicode damaged-previous)
if(NOT CASE IN_LIST _cases)
    message(FATAL_ERROR "Unknown receipt file case")
endif()
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _nonce)
set(_root "${WORK}/${_nonce}")
if(CASE STREQUAL "unicode")
    string(APPEND _root "/résultats with spaces")
endif()
file(MAKE_DIRECTORY "${_root}")
set(_manifest "${_root}/current.tsv")
set(_state "${_root}/receipts.tsv")
set(_excluded "${_root}/reused.txt")
string(REPEAT a 64 _digest)
set(_header "UMICOM-TEST-REUSE 1 ${_digest}")
set(_text "${_header}\nfixture.alpha\t${_digest}\t0\n")
set(_old "${_header}\nfixture.alpha\t${_digest}\t2\n")
set(_expected_reuse "fixture.alpha\n")
set(_status 0)
set(_newlines LF)
if(CASE STREQUAL "crlf" OR CASE STREQUAL "unicode")
    set(_newlines CRLF)
elseif(CASE STREQUAL "empty")
    set(_text "${_header}\n")
    set(_expected_reuse "")
elseif(CASE STREQUAL "bare-cr")
    set(_text "${_header}\nfixture.al\rpha\t${_digest}\t0\n")
    set(_status 1)
elseif(CASE STREQUAL "duplicate")
    string(APPEND _text "fixture.alpha\t${_digest}\t0\n")
    set(_status 1)
elseif(CASE STREQUAL "damaged-previous")
    set(_old "damaged receipt table\n")
    set(_expected_reuse "")
endif()
# Explicit file bytes make this a file-format boundary check on every host.
# Keep the prior state digest so rejected input must leave it untouched.
file(CONFIGURE OUTPUT "${_manifest}" CONTENT "${_text}" @ONLY NEWLINE_STYLE ${_newlines})
file(CONFIGURE OUTPUT "${_state}" CONTENT "${_old}" @ONLY NEWLINE_STYLE LF)
file(SHA256 "${_state}" _old_hash)
execute_process(COMMAND "${PROGRAM}" prepare "${_manifest}" "${_state}" "${_excluded}"
    RESULT_VARIABLE _actual OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 15)
if(NOT _actual STREQUAL "${_status}")
    message(FATAL_ERROR "prepare returned ${_actual}, expected ${_status}: ${_out}\n${_err}")
endif()
if(_status EQUAL 1)
    file(SHA256 "${_state}" _after_hash)
    if(NOT _old_hash STREQUAL _after_hash OR EXISTS "${_excluded}")
        message(FATAL_ERROR "Rejected current manifest changed state or exclusions")
    endif()
else()
    file(READ "${_excluded}" _selected)
    if(NOT _selected STREQUAL _expected_reuse)
        message(FATAL_ERROR "Unexpected reused tests: [${_selected}]")
    endif()
    file(READ "${_state}" _state_hex HEX)
    if(_state_hex MATCHES "0d")
        message(FATAL_ERROR "Receipt export must use canonical LF")
    endif()
    # The recorder must tolerate a report containing tests outside the closed
    # manifest without manufacturing receipts for those unknown input owners.
    set(_xml "<testsuite tests='2'><testcase name='fixture.alpha' status='run' time='0'/><testcase name='fixture.unknown' status='run' time='0'/></testsuite>")
    file(CONFIGURE OUTPUT "${_root}/results.xml" CONTENT "${_xml}" @ONLY NEWLINE_STYLE LF)
    execute_process(COMMAND "${PROGRAM}" record "${_state}" "${_root}/results.xml"
        RESULT_VARIABLE _recorded OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 15)
    if(NOT _recorded STREQUAL "0")
        message(FATAL_ERROR "record returned ${_recorded}: ${_out}\n${_err}")
    endif()
    file(READ "${_state}" _saved)
    if(_saved MATCHES "fixture[.]unknown")
        message(FATAL_ERROR "An undeclared test was incorrectly admitted to the cache")
    endif()
    if(NOT CASE STREQUAL "empty" AND NOT _saved MATCHES "fixture[.]alpha\t${_digest}\t2\n")
        message(FATAL_ERROR "The declared test did not retain its passing outcome")
    endif()
endif()
message(STATUS "Receipt file boundary checked: ${CASE}; evidence ${_root}")
