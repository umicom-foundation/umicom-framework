# Umicom Framework | Sammy Hegab | Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
execute_process(COMMAND "${CMAKE_COMMAND}" "-DCASE=${CASE}" "-DFRAMEWORK=${FRAMEWORK}"
    "-DWORK=${WORK}" -P "${CMAKE_CURRENT_LIST_DIR}/test_sourceview_resources.cmake"
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
if(_result STREQUAL "0" OR NOT "${_output}\n${_error}" MATCHES "Umicom GtkSourceView data is incomplete:")
    message(FATAL_ERROR "Expected the specific missing/invalid schema refusal; got ${_result}: ${_output}\n${_error}")
endif()
