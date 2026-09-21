#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/windows_deployment/expect_refusal.cmake
#
# PURPOSE:
#   Require the intended diagnostic, not merely any non-zero subprocess exit.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
set(_expected "")
if(CASE STREQUAL "bad-identifier")
    set(_expected "Unsafe or empty deployment identifier")
elseif(CASE MATCHES "^(bad-literal|bad-bracket|bad-newline|tab-path)$")
    set(_expected "Unsupported control/list delimiter")
elseif(CASE MATCHES "^(parent-path|absolute-path|drive-path|backslash-path|trailing-dot)$")
    set(_expected "Unsafe deployment-relative path")
elseif(CASE STREQUAL "reserved-path")
    set(_expected "Reserved Windows filename")
elseif(CASE MATCHES "^(duplicate-path|render-duplicate-path)$")
    set(_expected "Conflicting Windows package path")
elseif(CASE MATCHES "^(missing-file|directory-file|symlink-file)$")
    set(_expected "Runtime file is absent")
elseif(CASE STREQUAL "invalid-pe")
    set(_expected "Not a Windows PE image")
elseif(CASE STREQUAL "truncated-pe")
    set(_expected "Truncated Windows PE image")
elseif(CASE STREQUAL "wrong-architecture")
    set(_expected "Invalid PE signature or wrong architecture")
elseif(CASE STREQUAL "unowned-reset")
    set(_expected "Refusing to clear unowned package directory")
elseif(CASE STREQUAL "render-empty")
    set(_expected "Installer renderer requires UMI_SETUP_APPS")
elseif(CASE STREQUAL "render-missing")
    set(_expected "Missing installer component payload")
elseif(CASE STREQUAL "render-duplicate-title")
    set(_expected "Two applications would create the same shortcut")
elseif(CASE STREQUAL "render-invalid-version")
    set(_expected "Installer version must contain three numeric components")
elseif(CASE STREQUAL "render-control-file")
    set(_expected "Payload attempts to replace installer control metadata")
elseif(CASE STREQUAL "render-missing-exe")
    set(_expected "A regular built Windows executable is required")
elseif(CASE STREQUAL "render-bad-title")
    set(_expected "Invalid Windows shortcut/product title")
else()
    message(FATAL_ERROR "No expected refusal registered for ${CASE}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" "-DCASE=${CASE}"
    "-DFRAMEWORK=${FRAMEWORK}" "-DWORK=${WORK}" "-DFIXTURE_EXE=${FIXTURE_EXE}"
    -P "${CMAKE_CURRENT_LIST_DIR}/test_deployment.cmake"
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
# Normalise diagnostic wrapping before matching its meaning.
string(REGEX REPLACE "[ \t\r\n]+" " " _diagnostic "${_output} ${_error}")
string(FIND "${_diagnostic}" "${_expected}" _match)
if(_result STREQUAL "0" OR _match LESS 0)
    message(FATAL_ERROR
        "Expected refusal '${_expected}', got exit ${_result}:\n${_output}\n${_error}")
endif()
message(STATUS "Verified intended refusal: ${_expected}")
