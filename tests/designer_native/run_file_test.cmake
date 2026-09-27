# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Delete only this named CTest fixture, never a user-selected project.
if(NOT DEFINED ROOT OR NOT IS_DIRECTORY "${ROOT}" OR NOT DEFINED CASE OR NOT CASE MATCHES "^[a-z_]+$")
    message(FATAL_ERROR "Invalid isolated test fixture")
endif()
set(_output "${ROOT}/${CASE}")
file(REMOVE_RECURSE "${_output}")
execute_process(COMMAND "${TOOL}" "${CASE}" "${_output}" RESULT_VARIABLE _result)
if(NOT _result EQUAL 0)
    message(FATAL_ERROR "${CASE} failed: ${_result}")
endif()
