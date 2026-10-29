#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/release_impact/check_retry_cli.cmake
#
# PURPOSE:
#   Verify literal retry previews retain related fixtures and preserve inventory files.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
foreach(_required TOOL DEST ROOT CASE)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "Missing retry CLI input: ${_required}")
    endif()
endforeach()
set(_cases single duplicate related-reverse union unknown literal unicode missing-name invalid-option)
if(NOT CASE IN_LIST _cases)
    message(FATAL_ERROR "Unknown retry CLI scenario")
endif()
include("${ROOT}/cmake/UmicomReleaseInventoryFormat.cmake")
string(RANDOM LENGTH 12 ALPHABET 0123456789abcdef _nonce)
set(_run "${DEST}/run-${_nonce}")
if(CASE STREQUAL "unicode")
    string(APPEND _run "/résultats with spaces")
endif()
file(MAKE_DIRECTORY "${_run}")
set(_input "${_run}/inventory.tsv")
string(HEX "${_run}" _source)
string(HEX "${_run}/build" _build)
file(CONFIGURE OUTPUT "${_input}"
    CONTENT "UMICOM-RELEASE-INVENTORY\t1\ncontext\tcmake\t0123456789abcdef0123456789abcdef\t${_source}\t${_build}\t4465627567\n"
    @ONLY NEWLINE_STYLE LF)
foreach(_target base helper other)
    _umicom_inventory_row("${_input}" target "${_target}" "${_run}" configured
        "impact-version=1\ncomplete=1\nimpact-end\n")
endforeach()
_umicom_inventory_row("${_input}" source "${_run}/other.c" other present "synthetic")
set(_name base.test)
if(CASE STREQUAL "literal")
    set(_name "base.[one]+(two)")
elseif(CASE STREQUAL "unicode")
    set(_name "café.test")
endif()
string(HEX base.setup _related)
_umicom_inventory_row("${_input}" test "${_name}" "${_run}" registered
    "impact-version=2\ncomplete=1\ntarget=62617365\ntest=${_related}\nimpact-end\n")
_umicom_inventory_row("${_input}" test base.sibling "${_run}" registered
    "impact-version=1\ncomplete=1\ntarget=62617365\nimpact-end\n")
_umicom_inventory_row("${_input}" test base.setup "${_run}" registered
    "impact-version=1\ncomplete=1\ntarget=68656c706572\nimpact-end\n")
_umicom_inventory_row("${_input}" test other.test "${_run}" registered
    "impact-version=1\ncomplete=1\ntarget=6f74686572\nimpact-end\n")
file(SHA256 "${_input}" _before)
set(_arguments --inventory "${_input}" --retry-test "${_name}" --expression)
set(_expected 0)
set(_counts "Changed paths: 0; selected targets: 2; selected tests: 2.")
if(CASE STREQUAL "duplicate")
    list(APPEND _arguments --retry-test "${_name}")
elseif(CASE STREQUAL "related-reverse")
    set(_arguments --inventory "${_input}" --retry-test base.setup --expression)
elseif(CASE STREQUAL "union")
    list(APPEND _arguments --changed other.c)
    set(_counts "Changed paths: 1; selected targets: 3; selected tests: 3.")
elseif(CASE STREQUAL "unknown")
    set(_arguments --inventory "${_input}" --retry-test absent.test --expression)
    set(_counts "Changed paths: 0; selected targets: 3; selected tests: 4.")
elseif(CASE STREQUAL "missing-name")
    set(_arguments --inventory "${_input}" --retry-test)
    set(_expected 2)
elseif(CASE STREQUAL "invalid-option")
    list(APPEND _arguments --unsupported-option value)
    set(_expected 2)
endif()
execute_process(COMMAND "${TOOL}" impact ${_arguments}
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _errors TIMEOUT 30)
file(WRITE "${_run}/stdout.txt" "${_output}")
file(WRITE "${_run}/stderr.txt" "${_errors}")
file(SHA256 "${_input}" _after)
if(NOT _before STREQUAL _after OR NOT _result STREQUAL "${_expected}")
    message(FATAL_ERROR "Retry CLI result or inventory preservation failed; inspect ${_run}")
endif()
if(_expected EQUAL 0)
    foreach(_fragment "${_counts}" "PREVIEW ONLY")
        string(FIND "${_output}" "${_fragment}" _found)
        if(_found EQUAL -1)
            message(FATAL_ERROR "Missing retry preview evidence ${_fragment}; inspect ${_run}")
        endif()
    endforeach()
    if(CASE STREQUAL "unknown")
        if(NOT _output MATCHES "required test has no configured registration"
                OR NOT _output MATCHES "No narrow expression emitted")
            message(FATAL_ERROR "Unknown retry did not conservatively widen coverage")
        endif()
    else()
        if(NOT _output MATCHES "required-test" OR NOT _output MATCHES "related-test"
                OR _output MATCHES "base[.]sibling")
            message(FATAL_ERROR "Retry reasons or related selection differ")
        endif()
        if(CASE STREQUAL "literal")
            string(FIND "${_output}" "base\\.\\[one\\]\\+\\(two\\)" _found)
            if(_found EQUAL -1)
                message(FATAL_ERROR "Literal retry name was interpreted as a pattern")
            endif()
        endif()
    endif()
elseif(_output MATCHES "PREVIEW ONLY")
    message(FATAL_ERROR "Rejected arguments emitted a usable plan")
endif()
message(STATUS "Retry preview evidence retained at ${_run}")
