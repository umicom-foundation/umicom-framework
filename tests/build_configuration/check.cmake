# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
foreach(_required IN ITEMS ROOT DEST GENERATOR COMPILER KIND TESTING)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "Missing fixture argument: ${_required}")
    endif()
endforeach()

# Each test has its own binary directory, so parallel CTest runs cannot share
# caches. Save both output streams when configuration fails for the owner.
file(MAKE_DIRECTORY "${DEST}")
set(_arguments "-DROOT=${ROOT}" "-DBUILD_TESTING=${TESTING}" "-DFIXTURE_KIND=${KIND}")
if(KIND STREQUAL "diagnostic")
    list(APPEND _arguments "-DUMICOM_BUILD_NATIVE_TOOL=${NATIVE}" "-DFIXTURE_ORDER=${ORDER}")
elseif(KIND STREQUAL "quick-open")
    list(APPEND _arguments "-DFIXTURE_GTK=${GTK}" "-DFIXTURE_GLIB=${GLIB}")
else()
    message(FATAL_ERROR "Unknown fixture kind: ${KIND}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}"
    -S "${ROOT}/tests/build_configuration/fixture" -B "${DEST}"
    -G "${GENERATOR}" "-DCMAKE_C_COMPILER=${COMPILER}" ${_arguments}
    RESULT_VARIABLE _status OUTPUT_FILE "${DEST}/configure.txt"
    ERROR_FILE "${DEST}/configure-errors.txt")
if(NOT _status STREQUAL "0")
    message(FATAL_ERROR
        "Framework module composition failed; read ${DEST}/configure-errors.txt")
endif()
