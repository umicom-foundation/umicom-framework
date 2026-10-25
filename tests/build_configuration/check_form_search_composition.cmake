# Umicom Framework
# File: tests/build_configuration/check_form_search_composition.cmake
# PURPOSE: Keep module-composition diagnostics separate for each configuration.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
cmake_minimum_required(VERSION 3.24)
foreach(required IN ITEMS ROOT DEST GENERATOR COMPILER ORDER TESTING)
    if(NOT DEFINED ${required} OR "${${required}}" STREQUAL "")
        message(FATAL_ERROR "Missing composition argument: ${required}")
    endif()
endforeach()
file(MAKE_DIRECTORY "${DEST}")
execute_process(COMMAND "${CMAKE_COMMAND}"
    -S "${ROOT}/tests/build_configuration/form_search_composition" -B "${DEST}"
    -G "${GENERATOR}" "-DCMAKE_C_COMPILER=${COMPILER}"
    "-DROOT=${ROOT}" "-DFIXTURE_ORDER=${ORDER}" "-DBUILD_TESTING=${TESTING}"
    RESULT_VARIABLE result OUTPUT_FILE "${DEST}/configure.txt"
    ERROR_FILE "${DEST}/configure-errors.txt")
if(NOT result STREQUAL "0")
    message(FATAL_ERROR "Form/search composition failed; see ${DEST}/configure-errors.txt")
endif()
