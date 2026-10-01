#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/implementation_fingerprint/CMakeLists.txt
# PURPOSE: Exercise content identity and rebuilding after timestamp-preserving copies.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
foreach(_case source private-header relocation inputs refusals)
    add_test(NAME "framework.build.implementation-fingerprint.${_case}"
        COMMAND "${CMAKE_COMMAND}" "-DCASE=${_case}"
            "-DWORK_ROOT=${CMAKE_CURRENT_BINARY_DIR}/fixtures"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/check_content.cmake")
    set_tests_properties("framework.build.implementation-fingerprint.${_case}" PROPERTIES
        TIMEOUT 30 LABELS "framework;build;incremental;regression")
endforeach()

# The nested project compiles and executes a real consumer. A changed command
# must rebuild an old owner object even when the copied source has an old date.
if(NOT CMAKE_CROSSCOMPILING)
    add_test(NAME framework.build.implementation-fingerprint.incremental
        COMMAND "${CMAKE_COMMAND}"
            "-DWORK_ROOT=${CMAKE_CURRENT_BINARY_DIR}/fixtures"
            "-DCC=${CMAKE_C_COMPILER}" "-DGENERATOR=${CMAKE_GENERATOR}"
            "-DMAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/check_incremental.cmake")
    set_tests_properties(framework.build.implementation-fingerprint.incremental PROPERTIES
        TIMEOUT 180 LABELS "framework;build;incremental;link;regression")
endif()

# Git's Windows case folding must never hide a directory of product sources.
# Fixtures create their own small Git repository; they never stage or modify
# the contributor's checkout. Run both matching modes on every host.
find_package(Git QUIET)
if(Git_FOUND)
    foreach(_case true false)
        add_test(NAME "framework.build.source-delivery.ignorecase.${_case}"
            COMMAND "${CMAKE_COMMAND}" "-DGIT_EXECUTABLE=${GIT_EXECUTABLE}"
                "-DIGNORECASE=${_case}" "-DWORK_ROOT=${CMAKE_CURRENT_BINARY_DIR}/fixtures"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/check_source_delivery.cmake")
        set_tests_properties("framework.build.source-delivery.ignorecase.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;build;source-delivery;regression")
    endforeach()
else()
    message(STATUS "Source-delivery Git regression requires Git; it is not registered on this host.")
endif()
add_test(NAME framework.build.implementation-fingerprint.private-includes
    COMMAND "${CMAKE_COMMAND}" "-DWORK_ROOT=${CMAKE_CURRENT_BINARY_DIR}/fixtures"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/check_private_includes.cmake")
set_tests_properties(framework.build.implementation-fingerprint.private-includes PROPERTIES
    TIMEOUT 30 LABELS "framework;build;incremental;regression")

# Content assertions alone do not catch an invalid optional path probe. Make
# the existing include regression fail if CMake emits any warning, while
# preserving its normal exit-status and dependency-identity checks.
set_tests_properties(framework.build.implementation-fingerprint.private-includes PROPERTIES
    FAIL_REGULAR_EXPRESSION "CMake Warning")
