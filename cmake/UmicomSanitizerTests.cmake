# Umicom Framework
# File: cmake/UmicomSanitizerTests.cmake
# Purpose: Exercise sanitizer policy through isolated configure/link consumers.
# Author and organisation: Sammy Hegab, Umicom Foundation
# Licence: MIT
include_guard(GLOBAL)
set(sanitizer_cases disabled unsupported)
if(UMICOM_ENABLE_SANITIZERS)
    list(APPEND sanitizer_cases static-link)
endif()
foreach(case IN LISTS sanitizer_cases)
    add_test(NAME framework.build.sanitizers.${case}
        COMMAND "${CMAKE_COMMAND}"
            "-DSOURCE=${CMAKE_CURRENT_LIST_DIR}/../tests/sanitizers"
            "-DBINARY=${CMAKE_CURRENT_BINARY_DIR}/sanitizer-policy/${case}"
            "-DMODE=${case}"
            "-DCOMPILER=${CMAKE_C_COMPILER}"
            "-DGENERATOR=${CMAKE_GENERATOR}"
            "-DGENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DGENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            "-DMAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
            "-DCONFIG=$<CONFIG>"
            -P "${CMAKE_CURRENT_LIST_DIR}/../tests/sanitizers/check.cmake")
    set_tests_properties(framework.build.sanitizers.${case} PROPERTIES
        TIMEOUT 240 LABELS "framework;build_configuration;sanitizers")
endforeach()
