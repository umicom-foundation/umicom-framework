# Umicom Framework. Sammy Hegab, Umicom Foundation. MIT licence.
# Each native-form test gets isolated template and output folders. Fixtures are
# retained for inspection; a missing display is reported as skipped, not passed.
foreach(_required TEST_BINARY TEST_PROGRAM TEMPLATE_SOURCE CASE)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "Missing native creation argument: ${_required}")
    endif()
endforeach()
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _suffix)
set(_root "${TEST_BINARY}/${CASE}-${_suffix}")
file(MAKE_DIRECTORY "${_root}/output" "${_root}/templates")
file(COPY "${TEMPLATE_SOURCE}/" DESTINATION "${_root}/templates")
file(REAL_PATH "${_root}" _root)
execute_process(COMMAND "${TEST_PROGRAM}" "${CASE}" "${_root}"
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error TIMEOUT 40)
if(_result STREQUAL "77")
    message("SKIP: GTK display unavailable")
elseif(NOT _result STREQUAL "0")
    message(FATAL_ERROR "Application form ${CASE} failed (${_result}):\n${_output}\n${_error}")
endif()
