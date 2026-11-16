# Umicom Framework
# File: tests/native_launcher/check_composition.cmake
#
# PURPOSE:
#   Qualify the actual focused launcher dependency graph, native link and existing digest vectors without installation.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

cmake_minimum_required(VERSION 3.24)
if(NOT COMPOSITION MATCHES "^(native_launcher|setup_centre|boot_media)$" OR
   NOT IS_ABSOLUTE "${FRAMEWORK_SOURCE}" OR NOT IS_ABSOLUTE "${BUILD_ROOT}" OR
   NOT EXISTS "${COMPILER}" OR NOT EXISTS "${CTEST}")
    message(FATAL_ERROR "A supported composition and absolute source/build/tool paths are required")
endif()
set(_source "${FRAMEWORK_SOURCE}/examples/${COMPOSITION}")
if(NOT EXISTS "${_source}/CMakeLists.txt")
    message(FATAL_ERROR "Focused source project is missing")
endif()
file(MAKE_DIRECTORY "${BUILD_ROOT}")
set(_configure -S "${_source}" -B "${BUILD_ROOT}" -G "${GENERATOR}"
    "-DCMAKE_C_COMPILER=${COMPILER}" "-DCMAKE_INSTALL_PREFIX=${BUILD_ROOT}/unused-install"
    -DBUILD_TESTING=ON -DUMICOM_NATIVE_LAUNCHER_BUILD_TOOLS=OFF
    -DUMICOM_BOOT_MEDIA_ENABLE_DEVICE_WRITES=OFF)
if(MAKE_PROGRAM)
    list(APPEND _configure "-DCMAKE_MAKE_PROGRAM=${MAKE_PROGRAM}")
endif()
if(CONFIGURATION)
    list(APPEND _configure "-DCMAKE_BUILD_TYPE=${CONFIGURATION}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" ${_configure}
    RESULT_VARIABLE _configured OUTPUT_VARIABLE _stdout ERROR_VARIABLE _stderr TIMEOUT 90)
file(WRITE "${BUILD_ROOT}/configure-contract.log" "${_stdout}${_stderr}")
if(NOT _configured STREQUAL "0")
    message(FATAL_ERROR "Focused configuration failed: ${BUILD_ROOT}/configure-contract.log")
endif()
set(_config)
if(CONFIGURATION)
    set(_config --config "${CONFIGURATION}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --build "${BUILD_ROOT}" ${_config}
    --target umicom-native-launcher-test --parallel 2
    RESULT_VARIABLE _built OUTPUT_VARIABLE _stdout ERROR_VARIABLE _stderr TIMEOUT 90)
file(WRITE "${BUILD_ROOT}/build-contract.log" "${_stdout}${_stderr}")
if(NOT _built STREQUAL "0")
    message(FATAL_ERROR "Focused native link failed: ${BUILD_ROOT}/build-contract.log")
endif()
set(_test_config)
if(CONFIGURATION)
    set(_test_config -C "${CONFIGURATION}")
endif()
# The anchored SHA prefix avoids recursively running composition tests.
execute_process(COMMAND "${CTEST}" --test-dir "${BUILD_ROOT}" ${_test_config}
    -R "^framework[.]native_launcher[.]sha[.]" --output-on-failure --no-tests=error
    RESULT_VARIABLE _tested OUTPUT_VARIABLE _stdout ERROR_VARIABLE _stderr TIMEOUT 45)
file(WRITE "${BUILD_ROOT}/test-contract.log" "${_stdout}${_stderr}")
if(NOT _tested STREQUAL "0")
    message(FATAL_ERROR "Focused digest vectors failed: ${BUILD_ROOT}/test-contract.log")
endif()
