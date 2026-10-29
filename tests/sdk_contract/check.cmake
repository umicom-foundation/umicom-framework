# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Retain stdout/stderr from a real CMake configure, including expected refusals.
if(NOT DEFINED TEST_CASE OR NOT DEFINED FIXTURE OR NOT DEFINED OUTPUT_ROOT OR NOT DEFINED CONTRACT_HELPER OR NOT DEFINED GENERATOR)
    message(FATAL_ERROR "The installed SDK contract case needs explicit fixture paths")
endif()
if(NOT TEST_CASE MATCHES "^[a-z-]+$")
    message(FATAL_ERROR "Invalid contract case name")
endif()
file(MAKE_DIRECTORY "${OUTPUT_ROOT}")
# Retained one-shot check; repeated configuration must reject stale SDK headers on both runs.
if(FALSE)
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

endif()
# Reuse the same build directory deliberately: stale files must never become a
# trusted baseline merely because an earlier negative test left them behind.
set(_attempts 1)
if(TEST_CASE STREQUAL "extra-header")
    set(_attempts 2)
endif()
foreach(_attempt RANGE 1 ${_attempts})
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${FIXTURE}" -B "${OUTPUT_ROOT}/build" -G "${GENERATOR}"
    "-DTEST_CASE=${TEST_CASE}" "-DCONTRACT_HELPER=${CONTRACT_HELPER}"
    RESULT_VARIABLE _result OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 45)
file(WRITE "${OUTPUT_ROOT}/configure.log" "${_out}\n${_err}")
file(WRITE "${OUTPUT_ROOT}/configure-${_attempt}.log" "${_out}\n${_err}")
string(REGEX REPLACE "[ \t\r\n]+" " " _error_text "${_err}")
if(TEST_CASE STREQUAL "valid" OR TEST_CASE STREQUAL "spaced-prefix" OR
    TEST_CASE STREQUAL "transitive-cycle" OR TEST_CASE STREQUAL "transitive-link-only" OR
    TEST_CASE STREQUAL "external-target" OR TEST_CASE STREQUAL "inherited-headers" OR
    TEST_CASE STREQUAL "aggregate-headers" OR TEST_CASE STREQUAL "transitive-inherited-headers")
    if(NOT _result STREQUAL "0" OR NOT _out MATCHES "Installed Framework layout and public-header identity verified")
        message(FATAL_ERROR "Valid SDK fixture was not accepted; see ${OUTPUT_ROOT}/configure.log")
    endif()
else()
    if(_result STREQUAL "0" OR NOT DEFINED EXPECTED_ERROR OR NOT _error_text MATCHES "${EXPECTED_ERROR}")
        message(FATAL_ERROR "The expected SDK refusal was not observed; see ${OUTPUT_ROOT}/configure.log")
    endif()
endif()
endforeach()
