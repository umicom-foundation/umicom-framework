# Umicom Framework
# File: tests/toolchain/check_cvm_slices.cmake
#
# PURPOSE:
#   Check slice command boundaries, exact work counts and runtime failures.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
if(NOT DEFINED TOOL OR NOT EXISTS "${TOOL}")
    message(FATAL_ERROR "Select the built native preview command")
endif()
foreach(pair "1,4" "2,2" "3,2" "4,1" "256,1")
    string(REPLACE "," ";" fields "${pair}")
    list(GET fields 0 budget)
    list(GET fields 1 slices)
    execute_process(COMMAND "${TOOL}" --slice "${budget}"
        RESULT_VARIABLE result OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
    string(STRIP "${out}" out)
    if(NOT "${result}" STREQUAL "0" OR NOT "${out}" STREQUAL "slices=${slices} instructions=4 value=42")
        message(FATAL_ERROR "Slice ${budget} mismatch: ${result} / ${out} / ${err}")
    endif()
endforeach()
foreach(budget 0 257 -1 +2 1x 99999999999999999999999)
    execute_process(COMMAND "${TOOL}" --slice "${budget}"
        RESULT_VARIABLE result OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
    if(NOT "${result}" STREQUAL "2" OR NOT "${out}" STREQUAL "")
        message(FATAL_ERROR "Invalid budget did not refuse: ${budget}")
    endif()
endforeach()
execute_process(COMMAND "${TOOL}" --slice 4 "int main(void){return 1/0;}"
    RESULT_VARIABLE result OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
if(NOT "${result}" STREQUAL "1" OR NOT "${out}" STREQUAL "" OR NOT err MATCHES "1:")
    message(FATAL_ERROR "Runtime failure must not print a completed result: ${result} / ${out} / ${err}")
endif()
