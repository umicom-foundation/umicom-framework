# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
file(MAKE_DIRECTORY "${DEST}")
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${ROOT}/tests/release_inventory/fixture" -B "${DEST}" -G Ninja
    "-DROOT=${ROOT}" "-DCMAKE_C_COMPILER=${COMPILER}" -DCMAKE_BUILD_TYPE=Debug
    -DUMICOM_RELEASE_INVENTORY_CAPTURE=ON -DUMICOM_RELEASE_INVENTORY_COMPOSITION_TESTS=OFF
    -DUMICOM_RELEASE_BASELINE_COMPOSITION_TESTS=OFF -DUMICOM_RELEASE_BASELINE_RETAIN_LIFECYCLE=OFF
    "-DUMICOM_RELEASE_BASELINE_DEFER=${DEFER}"
    RESULT_VARIABLE _result OUTPUT_FILE "${DEST}/configure.txt" ERROR_FILE "${DEST}/configure-errors.txt")
if(NOT _result STREQUAL "0")
    message(FATAL_ERROR "Inventory fixture configure failed; see ${DEST}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --build "${DEST}" --parallel 2
    RESULT_VARIABLE _result OUTPUT_FILE "${DEST}/build.txt" ERROR_FILE "${DEST}/build-errors.txt")
if(NOT _result STREQUAL "0")
    message(FATAL_ERROR "Inventory fixture build failed; see ${DEST}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" "-DBUILD=${DEST}" -DCONFIG=Debug
    -P "${ROOT}/cmake/UmicomCaptureCTestInventory.cmake"
    RESULT_VARIABLE _result OUTPUT_FILE "${DEST}/capture.txt" ERROR_FILE "${DEST}/capture-errors.txt")
if(NOT _result STREQUAL "0")
    message(FATAL_ERROR "CTest capture failed; see ${DEST}")
endif()
set(_tool "${DEST}/baseline/bin/umicom-release-inventory")
if(WIN32)
    string(APPEND _tool ".exe")
endif()
# CMAKE_RUNTIME_OUTPUT_DIRECTORY in the focused owner uses CMAKE_BINARY_DIR.
if(NOT EXISTS "${_tool}")
    set(_tool "${DEST}/bin/umicom-release-inventory")
    if(WIN32)
        string(APPEND _tool ".exe")
    endif()
endif()
set(_configured "${DEST}/release-inventory/inventory-Debug.tsv")
set(_ctest "${DEST}/release-inventory/ctest-Debug.tsv")
execute_process(COMMAND "${_tool}" compare "${_configured}" "${_ctest}"
    RESULT_VARIABLE _result OUTPUT_FILE "${DEST}/compare.txt" ERROR_FILE "${DEST}/compare-errors.txt")
if(NOT _result STREQUAL "0")
    message(FATAL_ERROR "Exact configured/CTest names differ or are unavailable; see ${DEST}")
endif()
execute_process(COMMAND "${_tool}" list "${_configured}" RESULT_VARIABLE _result OUTPUT_VARIABLE _list)
if(NOT _result STREQUAL "0" OR NOT _list MATCHES "inventory.fixture.late"
        OR NOT _list MATCHES "inventory fixture \\[space\\]"
        OR NOT _list MATCHES "include/umicom/distribution/runtime/inventory.h\tumicom_distribution\tdeclared"
        OR NOT _list MATCHES "\tunassigned\t")
    message(FATAL_ERROR "Deferred registrations or explicit/unknown header ownership were lost")
endif()
# Same totals do not excuse a renamed registration.
file(READ "${_ctest}" _captured)
string(HEX "inventory.fixture.late" _old)
string(HEX "inventory.fixture.fake" _new)
string(REPLACE "test\t${_old}\t" "test\t${_new}\t" _changed "${_captured}")
file(WRITE "${DEST}/renamed.tsv" "${_changed}")
execute_process(COMMAND "${_tool}" compare "${_configured}" "${DEST}/renamed.tsv" RESULT_VARIABLE _result OUTPUT_QUIET)
if(NOT _result STREQUAL "1")
    message(FATAL_ERROR "A same-count registration change was not refused")
endif()
