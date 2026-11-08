#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/toolchain/check_cvm_preview_cli.cmake
#
# PURPOSE:
#   Check the optional preview process preserves ordinary output and exposes complete or rejected trace rows.
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
set(source "")
set(expected_exit 0)
set(expected "42\n")
set(require_error FALSE)
if(CASE STREQUAL "ordinary")
elseif(CASE STREQUAL "trace")
    set(expected "0 push depth=1 top=6 status=success\n1 push depth=2 top=7 status=success\n2 multiply depth=1 top=42 status=success\n3 return depth=1 top=42 status=success\n42\n")
elseif(CASE STREQUAL "trace-source")
    set(source "int main(void) { return 9; }")
    set(expected "0 push depth=1 top=9 status=success\n1 return depth=1 top=9 status=success\n9\n")
elseif(CASE STREQUAL "comparison-true")
    set(source "int main(void) { return 1 < 2; }")
    set(expected "0 push depth=1 top=1 status=success\n1 push depth=2 top=2 status=success\n2 less depth=1 top=1 status=success\n3 return depth=1 top=1 status=success\n1\n")
elseif(CASE STREQUAL "comparison-false")
    set(source "int main(void) { return 2 < 1; }")
    set(expected "0 push depth=1 top=2 status=success\n1 push depth=2 top=1 status=success\n2 less depth=1 top=0 status=success\n3 return depth=1 top=0 status=success\n0\n")
elseif(CASE STREQUAL "comparison-eager")
    set(source "int main(void) { return 0 == (1 / 0); }")
    set(expected_exit 1)
    set(require_error TRUE)
    set(expected "0 push depth=1 top=0 status=success\n1 push depth=2 top=1 status=success\n2 push depth=3 top=0 status=success\n3 divide depth=3 top=0 status=division by zero or signed arithmetic overflow\n")
elseif(CASE STREQUAL "trace-fault")
    set(source "int main(void) { return 7 / 0; }")
    set(expected_exit 1)
    set(require_error TRUE)
    set(expected "0 push depth=1 top=7 status=success\n1 push depth=2 top=0 status=success\n2 divide depth=2 top=0 status=division by zero or signed arithmetic overflow\n")
elseif(CASE STREQUAL "trace-syntax")
    set(source "int main(void) { return ; }")
    set(expected_exit 1)
    set(require_error TRUE)
    set(expected "")
elseif(CASE STREQUAL "extra-arguments")
    set(source "int main(void) { return 9; }")
    set(expected_exit 2)
    set(require_error TRUE)
    set(expected "")
else()
    message(FATAL_ERROR "Unknown preview CLI case")
endif()
# Keep source as one quoted argument: its C semicolon is not a CMake list item.
if(CASE STREQUAL "ordinary")
    execute_process(COMMAND "${TOOL}" RESULT_VARIABLE result
        OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 10)
elseif(CASE STREQUAL "trace")
    execute_process(COMMAND "${TOOL}" --trace RESULT_VARIABLE result
        OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 10)
elseif(CASE STREQUAL "extra-arguments")
    execute_process(COMMAND "${TOOL}" --trace "${source}" unexpected RESULT_VARIABLE result
        OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 10)
else()
    execute_process(COMMAND "${TOOL}" --trace "${source}" RESULT_VARIABLE result
        OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 10)
endif()
string(REPLACE "\r\n" "\n" output "${output}")
if(NOT "${result}" STREQUAL "${expected_exit}" OR NOT output STREQUAL expected)
    message(FATAL_ERROR "Unexpected preview outcome: ${result}: ${output}: ${error}")
endif()
if(require_error AND error STREQUAL "")
    message(FATAL_ERROR "Failed preview has no diagnostic")
elseif(NOT require_error AND NOT error STREQUAL "")
    message(FATAL_ERROR "Successful preview wrote an error: ${error}")
endif()
