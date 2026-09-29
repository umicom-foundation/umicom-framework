# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
file(MAKE_DIRECTORY "${DEST}")
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${ROOT}/tests/release_policy/header_fixture" -B "${DEST}" -G Ninja
    "-DROOT=${ROOT}" "-DCMAKE_C_COMPILER=${COMPILER}" -DCMAKE_BUILD_TYPE=Debug
    -DUMICOM_RELEASE_HEADER_CHECKS=ON -DUMICOM_RELEASE_INVENTORY_CAPTURE=ON
    -DUMICOM_RELEASE_INVENTORY_COMPOSITION_TESTS=OFF -DUMICOM_RELEASE_BASELINE_COMPOSITION_TESTS=OFF
    -DUMICOM_RELEASE_BASELINE_RETAIN_LIFECYCLE=OFF "-DUMICOM_RELEASE_BASELINE_DEFER=${DEFER}"
    RESULT_VARIABLE _result OUTPUT_FILE "${DEST}/configure.txt" ERROR_FILE "${DEST}/configure-errors.txt")
if(NOT _result STREQUAL "0")
    message(FATAL_ERROR "Public header fixture configure failed; see ${DEST}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --build "${DEST}" --target umicom-public-header-checks fixture-header-valid --parallel 2
    RESULT_VARIABLE _result OUTPUT_FILE "${DEST}/headers.txt" ERROR_FILE "${DEST}/headers-errors.txt")
if(NOT _result STREQUAL "0")
    message(FATAL_ERROR "Declared public headers did not compile independently; see ${DEST}")
endif()
file(READ "${DEST}/release-inventory/header-checks/manifest.tsv" _manifest)
if(NOT _manifest MATCHES "umicom/distribution/runtime/test_policy.h\tumicom_distribution"
        OR NOT _manifest MATCHES "umicom/base/status.h\tumicom_base"
        OR NOT _manifest MATCHES "umicom/base/version.h\tumicom_base")
    message(FATAL_ERROR "Declared-owner compile manifest is incomplete")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --build "${DEST}" --target fixture-header-invalid --parallel 2
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
file(WRITE "${DEST}/expected-failure.txt" "${_output}\n${_error}")
if(_result STREQUAL "0" OR NOT "${_output}\n${_error}" MATCHES "UmicomDeliberatelyMissingHeaderType")
    message(FATAL_ERROR "The missing public type was not diagnosed by the compiler")
endif()
