# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
file(MAKE_DIRECTORY "${DEST}")
execute_process(COMMAND "${CMAKE_COMMAND}"
    -S "${ROOT}/tests/bank_composition/fixture" -B "${DEST}" -G "${GENERATOR}"
    "-DROOT=${ROOT}" "-DCMAKE_C_COMPILER=${COMPILER}" "-DBUILD_TESTING=${TESTING}"
    RESULT_VARIABLE _status OUTPUT_FILE "${DEST}/configure.txt" ERROR_FILE "${DEST}/configure-errors.txt")
if(NOT _status STREQUAL "0")
    message(FATAL_ERROR "Optional bank composition failed; see ${DEST}/configure-errors.txt")
endif()
