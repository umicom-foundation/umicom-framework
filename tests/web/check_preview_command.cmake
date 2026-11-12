#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/web/check_preview_command.cmake
#
# PURPOSE:
#   Check the real native command entry with Unicode paths and refusal exit codes.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

if(NOT DEFINED TOOL OR NOT DEFINED CASE OR NOT DEFINED WORK)
    message(FATAL_ERROR "TOOL, CASE and WORK are required")
endif()
file(MAKE_DIRECTORY "${WORK}")
set(root "${WORK}/café static files")
set(port 8080)
set(expected 0)
if(CASE STREQUAL "relative")
    set(root "relative-site")
    set(expected 2)
elseif(CASE STREQUAL "invalid-port")
    set(port 65536)
    set(expected 2)
elseif(NOT CASE STREQUAL "unicode")
    message(FATAL_ERROR "Unknown preview case")
endif()
# Dry-run intentionally does not create/read the selected site or open a socket.
# Native commands emit UTF-8 regardless of the Windows console code page.
# Retain the implicit-decoding invocation for review; explicitly decode the
# child output so this fixture measures path preservation, not CMake policy.
if(FALSE)
execute_process(COMMAND "${TOOL}" preview --root "${root}" --port "${port}" --dry-run
    WORKING_DIRECTORY "${WORK}" RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error
    TIMEOUT 10)
endif()
execute_process(COMMAND "${TOOL}" preview --root "${root}" --port "${port}" --dry-run
    WORKING_DIRECTORY "${WORK}" RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error
    ENCODING UTF-8 TIMEOUT 10)
if(NOT "${result}" STREQUAL "${expected}")
    message(FATAL_ERROR "Expected exit ${expected}, got ${result}: ${output} ${error}")
endif()
if(CASE STREQUAL "unicode")
    string(FIND "${output}" "Root: ${root}" found)
    if(found EQUAL -1)
        message(FATAL_ERROR "The native command did not preserve the selected UTF-8 root: ${output}")
    endif()
endif()
