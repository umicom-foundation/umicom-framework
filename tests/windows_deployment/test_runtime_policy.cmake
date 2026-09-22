#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/windows_deployment/test_runtime_policy.cmake
#
# PURPOSE:
#   Verify the policy scope of the real runtime-staging script without scanning
#   Windows DLLs or changing a deployed application. Newer CMake hosts check
#   CMP0207 NEW; older hosts exercise the unknown-policy compatibility guard.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(NOT FRAMEWORK OR NOT EXISTS "${FRAMEWORK}/cmake/windows/StageRuntime.cmake")
    message(FATAL_ERROR "FRAMEWORK must identify the checkout under test")
endif()
if(NOT WORK)
    message(FATAL_ERROR "WORK must identify a disposable test-output directory")
endif()
if(NOT MODE STREQUAL "default" AND NOT MODE STREQUAL "inherited-old")
    message(FATAL_ERROR "MODE must be default or inherited-old")
endif()

file(MAKE_DIRECTORY "${WORK}")
set(_probe "${WORK}/policy-probe.cmake")
# StageRuntime includes its generated configuration before checking the host
# platform. Use that existing boundary to inspect the actual script's policy.
# The explicit sentinel terminates before any runtime file can be copied.
file(WRITE "${_probe}" [=[
if(POLICY CMP0207)
    cmake_policy(GET CMP0207 _selected)
    if(NOT _selected STREQUAL "NEW")
        message(FATAL_ERROR "UMICOM_POLICY_PROBE_WRONG_POLICY: ${_selected}")
    endif()
    message(STATUS "CMP0207 NEW verified in the staging script process")
else()
    message(STATUS "CMP0207 unavailable: older-CMake compatibility guard exercised")
endif()
message(FATAL_ERROR "UMICOM_RUNTIME_POLICY_PROBE_COMPLETE")
]=])
set(_options "")
if(MODE STREQUAL "inherited-old")
    # An external compatibility default must not override the staging script's
    # deliberate selection. Unknown defaults are harmless on older CMake.
    list(APPEND _options "-DCMAKE_POLICY_DEFAULT_CMP0207=OLD")
endif()
execute_process(
    COMMAND "${CMAKE_COMMAND}" ${_options}
        "-DUMI_DEPLOY_CONFIG=${_probe}"
        -P "${FRAMEWORK}/cmake/windows/StageRuntime.cmake"
    RESULT_VARIABLE _result
    OUTPUT_VARIABLE _stdout
    ERROR_VARIABLE _stderr
    TIMEOUT 20)
file(WRITE "${WORK}/policy-probe.log"
    "CMake: ${CMAKE_VERSION}\nMode: ${MODE}\nResult: ${_result}\n"
    "${_stdout}\n${_stderr}\n")
if(_result STREQUAL "0" OR
   NOT _stderr MATCHES "UMICOM_RUNTIME_POLICY_PROBE_COMPLETE" OR
   _stderr MATCHES "UMICOM_POLICY_PROBE_WRONG_POLICY")
    message(FATAL_ERROR "Staging policy probe failed:\n${_stdout}\n${_stderr}")
endif()
if(POLICY CMP0207)
    if(NOT _stdout MATCHES "CMP0207 NEW verified")
        message(FATAL_ERROR "The available CMP0207 policy was not exercised")
    endif()
else()
    if(NOT _stdout MATCHES "older-CMake compatibility guard exercised")
        message(FATAL_ERROR "The older-CMake compatibility path was not exercised")
    endif()
endif()
message(STATUS "Runtime policy scope test passed (${MODE}); no DLLs were staged")
