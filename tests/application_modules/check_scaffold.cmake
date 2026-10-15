# Umicom Framework: isolated source-creator regression fixtures.
# Sammy Hegab, Umicom Foundation. MIT licence.
foreach(_required TEST_BINARY TEST_PROGRAM TEMPLATE_SOURCE CASE)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "Missing scaffold test argument: ${_required}")
    endif()
endforeach()
# Retain previous fixtures and give each run its own directory. This makes
# concurrent tests independent without deleting a caller-provided path.
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _suffix)
set(_root "${TEST_BINARY}/${CASE}-${_suffix}")
file(MAKE_DIRECTORY "${_root}/output" "${_root}/templates")
file(COPY "${TEMPLATE_SOURCE}/" DESTINATION "${_root}/templates")
file(REAL_PATH "${_root}" _root)
execute_process(COMMAND "${TEST_PROGRAM}" "${CASE}" "${_root}"
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error TIMEOUT 30)
if(NOT _result STREQUAL "0")
    message(FATAL_ERROR "Scaffold ${CASE} failed (${_result}):\n${_output}\n${_error}")
endif()
