#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/development_workflow/check_cli_debug.cmake
#
# PURPOSE:
#   Exercise the native command entry point and verify its stopped source.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
function(check_admission expected marker)
    execute_process(COMMAND "${TOOL}" debug ${ARGN} WORKING_DIRECTORY "${SOURCE}"
        TIMEOUT 5 RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
    if(NOT "${result}" STREQUAL "${expected}" OR NOT "${output}${error}" MATCHES "${marker}")
        message(FATAL_ERROR "Unexpected debugger admission result ${result}: ${output}${error}")
    endif()
    if(NOT "${result}" STREQUAL "0" AND NOT output STREQUAL "")
        message(FATAL_ERROR "Debugger refusal published JSON: ${output}")
    endif()
endfunction()

if(ADMISSION)
    check_admission(0 "Usage: umicom debug" --help)
    check_admission(2 "Usage: umicom debug" --program)
    check_admission(2 "Usage: umicom debug" --program no-file --program duplicate)
    check_admission(2 "Usage: umicom debug" --program no-file --kind other)
    check_admission(2 "Usage: umicom debug" --program no-file --timeout-ms 30001)
    check_admission(1 "debug launch failed" --program missing-umicom-debuggee-executable)
    return()
endif()

execute_process(COMMAND "${TOOL}" debug --kind "${KIND}" --adapter "${DEBUGGER}"
    --source "${SOURCE}" --program "${PROGRAM}" --timeout-ms 8000 -- "two words" "--literal"
    TIMEOUT 35 RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
if(NOT "${result}" STREQUAL "0")
    message(FATAL_ERROR "Native debug command failed (${result}): ${output}${error}")
endif()
string(JSON event GET "${output}" event)
string(JSON frame GET "${output}" frame)
string(JSON path GET "${output}" source)
string(JSON line GET "${output}" line)
if(NOT event STREQUAL "stopped" OR NOT frame STREQUAL "main" OR
        NOT path MATCHES "notes_debuggee[.]c$" OR line LESS 9 OR line GREATER 17)
    message(FATAL_ERROR "Debugger did not report the Notes main frame: ${output}")
endif()
message(STATUS "Observed native frame: ${output}")
