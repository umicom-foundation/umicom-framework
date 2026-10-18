# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Retain stdout/stderr from a real CMake configure, including expected refusals.
if(NOT DEFINED TEST_CASE OR NOT DEFINED FIXTURE OR NOT DEFINED OUTPUT_ROOT OR NOT DEFINED CONTRACT_HELPER OR NOT DEFINED GENERATOR)
    message(FATAL_ERROR "The installed SDK contract case needs explicit fixture paths")
endif()
if(NOT TEST_CASE MATCHES "^[a-z-]+$")
    message(FATAL_ERROR "Invalid contract case name")
endif()
file(MAKE_DIRECTORY "${OUTPUT_ROOT}")
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${FIXTURE}" -B "${OUTPUT_ROOT}/build" -G "${GENERATOR}"
    "-DTEST_CASE=${TEST_CASE}" "-DCONTRACT_HELPER=${CONTRACT_HELPER}"
    RESULT_VARIABLE _result OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 45)
file(WRITE "${OUTPUT_ROOT}/configure.log" "${_out}\n${_err}")
string(REGEX REPLACE "[ \t\r\n]+" " " _error_text "${_err}")
if(TEST_CASE STREQUAL "valid" OR TEST_CASE STREQUAL "spaced-prefix")
    if(NOT _result STREQUAL "0" OR NOT _out MATCHES "Installed Framework layout and public-header identity verified")
        message(FATAL_ERROR "Valid SDK fixture was not accepted; see ${OUTPUT_ROOT}/configure.log")
    endif()
else()
    if(_result STREQUAL "0" OR NOT DEFINED EXPECTED_ERROR OR NOT _error_text MATCHES "${EXPECTED_ERROR}")
        message(FATAL_ERROR "The expected SDK refusal was not observed; see ${OUTPUT_ROOT}/configure.log")
    endif()
endif()
