#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAutomaticTestsRun.cmake
#
# PURPOSE:
#   Build incrementally and execute tests whose passing evidence cannot be reused.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.29)
# Use canonical dependency paths on CMake versions which provide this policy.
# Older supported versions retain their existing dependency discovery.
if(POLICY CMP0207)
    cmake_policy(SET CMP0207 NEW)
endif()
if(NOT DEFINED UMICOM_AUTOMATIC_CONFIGURATION)
    set(UMICOM_AUTOMATIC_CONFIGURATION "Debug")
endif()
if(NOT UMICOM_AUTOMATIC_CONFIGURATION MATCHES "^[A-Za-z0-9_-]+$")
    message(FATAL_ERROR "Automatic test configuration must be a portable identifier")
endif()
set(_build "${UMICOM_AUTOMATIC_BUILD_ROOT}")
set(_ctest "${UMICOM_AUTOMATIC_CTEST}")
set(_config "${UMICOM_AUTOMATIC_CONFIGURATION}")
set(_home "${_build}/.umicom/qualification/${_config}")
file(MAKE_DIRECTORY "${_home}")
file(LOCK "${_build}/.umicom/qualification.lock" GUARD PROCESS TIMEOUT 0 RESULT_VARIABLE _lock)
if(NOT _lock STREQUAL "0")
    message(FATAL_ERROR "Another automatic qualification owns this build directory: ${_lock}")
endif()
string(TIMESTAMP _time "%Y%m%dT%H%M%SZ" UTC)
string(RANDOM LENGTH 12 ALPHABET 0123456789abcdef _suffix)
set(_run "${_home}/runs/${_time}-${_suffix}")
file(MAKE_DIRECTORY "${_run}")
message(STATUS "Automatic qualification logs: ${_run}")
# Refresh content fingerprints even when copied source files retained old
# timestamps. Existing cache choices are retained; no -D defaults are imposed.
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${UMICOM_AUTOMATIC_SOURCE_ROOT}" -B "${_build}"
    OUTPUT_FILE "${_run}/configure.log" ERROR_FILE "${_run}/configure.log" RESULT_VARIABLE _configured)
if(NOT _configured STREQUAL "0")
    message(FATAL_ERROR "Configure failed (${_configured}); read ${_run}/configure.log")
endif()
# Reuse the generator's dependency database. No clean, timestamp touch or new
# build directory is requested. This also makes direct driver invocation safe.
execute_process(COMMAND "${CMAKE_COMMAND}" --build "${_build}" --config "${_config}" --parallel 2
    OUTPUT_FILE "${_run}/incremental-build.log" ERROR_FILE "${_run}/incremental-build.log" RESULT_VARIABLE _built)
if(NOT _built STREQUAL "0")
    message(FATAL_ERROR "Incremental build failed (${_built}); no tests or receipts were accepted")
endif()
if(NOT EXISTS "${UMICOM_AUTOMATIC_RECEIPTS}")
    message(FATAL_ERROR "The native test-receipt helper was not built")
endif()

# The script and executable may have been integrated at different times. Check
# their command contract before any previous passing result can exclude a test.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomTestReceiptProtocol.cmake")
umicom_check_test_receipt_protocol("${UMICOM_AUTOMATIC_RECEIPTS}"
    "${_run}/receipt-helper-protocol" _protocol_error)
if(NOT _protocol_error STREQUAL "")
    message(FATAL_ERROR "${_protocol_error}; no tests or receipts were accepted")
endif()

function(_fingerprint output discovery)
    execute_process(COMMAND "${_ctest}" --test-dir "${_build}" -C "${_config}" --show-only=json-v1
        OUTPUT_FILE "${discovery}" ERROR_FILE "${discovery}.errors.txt" RESULT_VARIABLE _discovered)
    if(NOT _discovered STREQUAL "0")
        message(FATAL_ERROR "CTest discovery failed; read ${discovery}.errors.txt")
    endif()
    file(READ "${discovery}" _json)
    string(JSON _count ERROR_VARIABLE _error LENGTH "${_json}" tests)
    if(NOT _error STREQUAL "NOTFOUND" OR _count EQUAL 0)
        message(FATAL_ERROR "No complete nonempty CTest catalogue was discovered")
    endif()
    file(READ "${_build}/umicom/closed-tests-${_config}.tsv" _registry)
    set(_owners "")
    string(REPLACE "\n" ";" _rows "${_registry}")
    foreach(_row IN LISTS _rows)
        if(_row STREQUAL "")
            continue()
        endif()
        string(REPLACE "\t" ";" _fields "${_row}")
        list(LENGTH _fields _fields_count)
        if(NOT _fields_count EQUAL 2)
            message(FATAL_ERROR "Malformed closed-input registration")
        endif()
        list(GET _fields 1 _owner)
        if(NOT EXISTS "${_owner}" OR IS_DIRECTORY "${_owner}")
            message(FATAL_ERROR "Missing test executable: ${_owner}")
        endif()
        list(APPEND _owners "${_owner}")
    endforeach()
    list(REMOVE_DUPLICATES _owners)
    list(SORT _owners)
    set(_runtime "")
    set(_unresolved "")
    set(_conflicting_FILENAMES "")
    set(_can_reuse TRUE)
    set(_reuse_reason "complete runtime input closure")
    if(UMICOM_AUTOMATIC_TEST_FULL OR UMICOM_AUTOMATIC_RUNTIME_PLATFORM STREQUAL ""
            OR NOT EXISTS "${UMICOM_AUTOMATIC_OBJDUMP}")
        set(_can_reuse FALSE)
        set(_reuse_reason "full execution requested or runtime inspection unavailable")
    elseif(_owners)
        # Bind dynamically loaded link dependencies as well as the executable.
        # Runtime plugins/data not visible to the linker cannot use this contract.
        set(CMAKE_GET_RUNTIME_DEPENDENCIES_PLATFORM "${UMICOM_AUTOMATIC_RUNTIME_PLATFORM}")
        set(CMAKE_GET_RUNTIME_DEPENDENCIES_TOOL "objdump")
        set(CMAKE_GET_RUNTIME_DEPENDENCIES_COMMAND "${UMICOM_AUTOMATIC_OBJDUMP}")
        file(GET_RUNTIME_DEPENDENCIES EXECUTABLES ${_owners}
            DIRECTORIES "${UMICOM_AUTOMATIC_COMPILER_DIR}"
            RESOLVED_DEPENDENCIES_VAR _runtime UNRESOLVED_DEPENDENCIES_VAR _unresolved
            CONFLICTING_DEPENDENCIES_PREFIX _conflicting)
        if(_unresolved OR _conflicting_FILENAMES)
            message(STATUS "Runtime dependencies are incomplete; executing all tests")
            set(_reuse_reason "unresolved or conflicting runtime dependencies")
            set(_can_reuse FALSE)
        endif()
    endif()
    # The owner target is not proof that CTest launches it. Check decoded argv[0]
    # against the complete registry before admitting any historical pass. A
    # wrapper or missing/ambiguous command keeps every test selected.
    execute_process(COMMAND "${UMICOM_AUTOMATIC_RECEIPTS}" verify-inputs
        "${_build}/umicom/closed-tests-${_config}.tsv" "${discovery}"
        OUTPUT_FILE "${discovery}.owners.log" ERROR_FILE "${discovery}.owners.log"
        RESULT_VARIABLE _owners_verified TIMEOUT 60)
    if(NOT _owners_verified STREQUAL "0")
        set(_can_reuse FALSE)
        set(_reuse_reason "registered executable ownership could not be verified")
    endif()
    file(SHA256 "${discovery}" _catalogue_hash)
    file(SHA256 "${_build}/CMakeCache.txt" _cache_hash)
    file(SHA256 "${UMICOM_AUTOMATIC_RECEIPTS}" _helper_hash)
    file(SHA256 "${UMICOM_AUTOMATIC_SCRIPT}" _driver_hash)
    # Do not save or print environment values: only their digest is retained.
    execute_process(COMMAND "${CMAKE_COMMAND}" -E environment OUTPUT_VARIABLE _environment RESULT_VARIABLE _env_status)
    if(NOT _env_status STREQUAL "0")
        message(FATAL_ERROR "Could not fingerprint the test environment")
    endif()
    string(SHA256 _environment_hash "${_environment}")
    set(_context "${_catalogue_hash}|${_cache_hash}|${_helper_hash}|${_driver_hash}|${_environment_hash}|${_registry}")
    list(SORT _runtime)
    foreach(_file IN LISTS _runtime)
        file(SHA256 "${_file}" _digest)
        string(APPEND _context "|${_file}|${_digest}")
    endforeach()
    string(SHA256 _context "${_context}")
    set(_manifest "UMICOM-TEST-REUSE 1 ${_context}\n")
    set(_failures "")
    foreach(_failed_file "${_build}/Testing/Temporary/LastTestsFailed.log"
            "${_build}/Testing/Temporary/LastTestsFailed_${_config}.log")
        if(EXISTS "${_failed_file}")
            file(STRINGS "${_failed_file}" _failed_lines)
            foreach(_line IN LISTS _failed_lines)
                if(_line MATCHES "^[0-9]+:(.+)$")
                    list(APPEND _failures "${CMAKE_MATCH_1}")
                else()
                    set(_reuse_reason "malformed external failure history")
                    set(_can_reuse FALSE)
                endif()
            endforeach()
        endif()
    endforeach()
    if(_can_reuse)
        # Hash each shared executable only once even when it owns many cases.
        foreach(_owner IN LISTS _owners)
            string(SHA256 _key "${_owner}")
            file(SHA256 "${_owner}" "_binary_${_key}")
        endforeach()
        foreach(_row IN LISTS _rows)
            if(_row STREQUAL "")
                continue()
            endif()
            string(REPLACE "\t" ";" _fields "${_row}")
            list(GET _fields 0 _name)
            list(GET _fields 1 _owner)
            string(SHA256 _key "${_owner}")
            set(_outcome 0)
            if(_name IN_LIST _failures)
                set(_outcome 3)
            endif()
            string(APPEND _manifest "${_name}\t${_binary_${_key}}\t${_outcome}\n")
        endforeach()
    endif()
    # Explain conservative fallback without exposing environment values. These
    # records are evidence, never an allow-list that grants permission to skip.
    file(WRITE "${output}.runtime.txt"
        "reason=${_reuse_reason}\nresolved=${_runtime}\nunresolved=${_unresolved}\nconflicts=${_conflicting_FILENAMES}\n")
    set(_reuse_mode full)
    if(_can_reuse)
        set(_reuse_mode eligible)
    endif()
    file(WRITE "${output}.policy.txt" "${_reuse_mode}\n")
    file(WRITE "${output}" "${_manifest}")
endfunction()

_fingerprint("${_run}/before.tsv" "${_run}/discovery.json")
execute_process(COMMAND "${UMICOM_AUTOMATIC_RECEIPTS}" prepare "${_run}/before.tsv"
    "${_home}/receipts.tsv" "${_run}/reused-tests.txt" RESULT_VARIABLE _prepared)
if(NOT _prepared STREQUAL "0")
    message(FATAL_ERROR "Receipt preparation failed; no tests were skipped")
endif()
execute_process(COMMAND "${_ctest}" --test-dir "${_build}" -C "${_config}"
    --exclude-from-file "${_run}/reused-tests.txt" --show-only=json-v1
    OUTPUT_FILE "${_run}/selected.json" RESULT_VARIABLE _selection)
if(NOT _selection STREQUAL "0")
    message(FATAL_ERROR "Selected-test discovery failed")
endif()
file(READ "${_run}/selected.json" _selected_json)
string(JSON _selected_count ERROR_VARIABLE _selected_error LENGTH "${_selected_json}" tests)
if(NOT _selected_error STREQUAL "NOTFOUND")
    message(FATAL_ERROR "Selected-test catalogue is invalid")
endif()
if(_selected_count EQUAL 0)
    message(STATUS "No tests need execution. Reused results are historical evidence, not fresh passes.")
    return()
endif()
message(STATUS "Executing ${_selected_count} tests; unknown inputs and previous nonpasses remain selected")
execute_process(COMMAND "${_ctest}" --test-dir "${_build}" -C "${_config}"
    --exclude-from-file "${_run}/reused-tests.txt" --output-on-failure --no-tests=error
    --output-log "${_run}/tests.log" --output-junit "${_run}/results.xml" RESULT_VARIABLE _tested)
# Outcomes alter LastTestsFailed.log; compare input context/digests, not the
# external failure marker. Native receipts retain actual per-test outcomes.
_fingerprint("${_run}/after.tsv" "${_run}/discovery-after.json")
file(READ "${_run}/before.tsv" _before)
file(READ "${_run}/after.tsv" _after)
string(REGEX REPLACE "\t[0-6]\n" "\t0\n" _before "${_before}")
string(REGEX REPLACE "\t[0-6]\n" "\t0\n" _after "${_after}")
if(NOT _before STREQUAL _after)
    message(FATAL_ERROR "Inputs changed during testing. Selected receipts remain unqualified; retry with stable inputs.")
endif()
if(EXISTS "${_run}/results.xml")
    # Keep the report-only call for review. The process result is now part of
    # receipt admission: failed/unknown runner exits cannot bless fresh passes.
    if(FALSE)
    execute_process(COMMAND "${UMICOM_AUTOMATIC_RECEIPTS}" record "${_home}/receipts.tsv"
        "${_run}/results.xml" RESULT_VARIABLE _recorded)
    endif()
    set(_runner_state failure)
    if(_tested STREQUAL "0")
        set(_runner_state success)
    endif()
    execute_process(COMMAND "${UMICOM_AUTOMATIC_RECEIPTS}" record-result "${_home}/receipts.tsv"
        "${_run}/results.xml" "${_runner_state}" RESULT_VARIABLE _recorded)
    if(NOT _recorded STREQUAL "0")
        message(FATAL_ERROR "Test report could not be recorded; selected tests remain due")
    endif()
else()
    message(FATAL_ERROR "CTest produced no result report; selected tests remain due")
endif()
if(NOT _tested STREQUAL "0")
    message(FATAL_ERROR "CTest failed (${_tested}); individual outcomes and the retry state were retained")
endif()
message(STATUS "Automatic qualification finished. Inspect ${_run}/results.xml for passes, skips and failures.")
