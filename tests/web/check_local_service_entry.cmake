#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/web/check_local_service_entry.cmake
#
# PURPOSE:
#   Drive native example entry cases from a different working directory using real Unicode file names.
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
set(root "${WORK}/café selected site")
set(port 8080)
set(expected 0)
if(CASE STREQUAL "ascii")
    set(root "${WORK}/selected site")
elseif(CASE STREQUAL "relative")
    set(root "relative-site")
    set(expected 2)
elseif(CASE STREQUAL "invalid-port")
    set(port 65536)
    set(expected 2)
elseif(NOT CASE STREQUAL "unicode")
    message(FATAL_ERROR "Unknown native example entry case")
endif()
file(MAKE_DIRECTORY "${WORK}/unrelated")
if(expected EQUAL 0)
    file(MAKE_DIRECTORY "${root}")
    file(WRITE "${root}/message.txt" "Unicode root reached.\n")
endif()
execute_process(COMMAND "${TOOL}" --port "${port}" --root "${root}"
    WORKING_DIRECTORY "${WORK}/unrelated" RESULT_VARIABLE result
    OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 10)
if(NOT "${result}" STREQUAL "${expected}")
    message(FATAL_ERROR "Expected exit ${expected}, got ${result}: ${output} ${error}")
endif()
