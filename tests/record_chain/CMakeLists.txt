# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomRecordChainQualification LANGUAGES C)
include(CTest)
get_filename_component(_root "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
# Compile the real shared hash implementation; this is not a mock SDK consumer.
add_library(umicom_record_chain_base STATIC "${_root}/src/base/sha256.c" "${_root}/src/base/status.c")
add_library(Umicom::base ALIAS umicom_record_chain_base)
set_target_properties(umicom_record_chain_base PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO EXPORT_NAME base)
target_include_directories(umicom_record_chain_base PUBLIC
    "$<BUILD_INTERFACE:${_root}/include>" "$<INSTALL_INTERFACE:include>")
include("${_root}/cmake/UmicomRecordChain.cmake")
install(TARGETS umicom_record_chain_base EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION lib COMPONENT FrameworkSDK)
install(FILES "${_root}/include/umicom/base/sha256.h" "${_root}/include/umicom/base/status.h"
    DESTINATION include/umicom/base COMPONENT FrameworkSDK)
install(EXPORT UmicomFrameworkTargets NAMESPACE Umicom:: DESTINATION lib/cmake/UmicomFramework
    COMPONENT FrameworkSDK)
# A minimal qualification package exports the same production receipt library.
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/UmicomFrameworkConfig.cmake"
    "include(\"\${CMAKE_CURRENT_LIST_DIR}/UmicomFrameworkTargets.cmake\")\n")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/UmicomFrameworkConfig.cmake"
    DESTINATION lib/cmake/UmicomFramework COMPONENT FrameworkSDK)
if(BUILD_TESTING)
    add_test(NAME framework.record_chain.installed_consumer COMMAND "${CMAKE_COMMAND}"
        "-DROOT=${_root}" "-DBUILD=${CMAKE_CURRENT_BINARY_DIR}"
        "-DCONFIG=$<CONFIG>" "-DGENERATOR=${CMAKE_GENERATOR}"
        "-DCOMPILER=${CMAKE_C_COMPILER}" "-DCTEST_PROGRAM=${CMAKE_CTEST_COMMAND}" -P "${CMAKE_CURRENT_SOURCE_DIR}/installed.cmake")
    set_tests_properties(framework.record_chain.installed_consumer PROPERTIES
        TIMEOUT 120 LABELS "framework;record-chain;sdk;installed-consumer" RUN_SERIAL TRUE)
endif()
