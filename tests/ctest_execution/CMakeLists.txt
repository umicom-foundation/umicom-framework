# -----------------------------------------------------------------------------
# Umicom Framework
# File: tests/ctest_execution/CMakeLists.txt
# Author: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
# Purpose: Build focused CTest adapter checks against actual Framework sources.
# This is a standalone test project, not a new product or runtime library.
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.21)
project(UmicomCtestExecutionChecks LANGUAGES C)
include(CTest)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
get_filename_component(framework_root "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
option(UMICOM_CTEST_CHECKS_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    if(UMICOM_CTEST_CHECKS_SANITIZERS)
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
    endif()
elseif(MSVC)
    add_compile_options(/W4 /WX)
endif()

set(shared_sources
    "${framework_root}/src/platform/cancellation.c"
    "${framework_root}/src/testing/case.c"
    "${framework_root}/src/testing/suite.c"
    "${framework_root}/src/testing/discovery.c"
    "${framework_root}/src/testing/ctest_adapter.c")
add_library(umicom-ctest-checks-core STATIC ${shared_sources})
target_include_directories(umicom-ctest-checks-core PUBLIC "${framework_root}/include")
add_library(umicom-ctest-checks-process STATIC "${framework_root}/src/platform/process.c")
target_include_directories(umicom-ctest-checks-process PUBLIC "${framework_root}/include")
foreach(kind report execution suite mutation report_failures discovery discovery_process discovery_native)
    add_executable(umicom-ctest-${kind}-checks "test_${kind}.c")
    target_link_libraries(umicom-ctest-${kind}-checks PRIVATE umicom-ctest-checks-core)
    if(NOT kind STREQUAL "report_failures" AND NOT kind STREQUAL "discovery_process")
        target_link_libraries(umicom-ctest-${kind}-checks PRIVATE umicom-ctest-checks-process)
    endif()
endforeach()
add_executable(umicom-ctest-fixture-helper fixture_helper.c)
# The inner helper intentionally crashes/exits/fails in selected scenarios.
# Do not instrument it: sanitizer diagnostics must concern the adapter, not a
# deliberately failing child. An executable built below retains normal flags.
if(UMICOM_CTEST_CHECKS_SANITIZERS AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-ctest-fixture-helper PRIVATE -fno-sanitize=all)
    target_link_options(umicom-ctest-fixture-helper PRIVATE -fno-sanitize=all)
endif()
add_custom_target(umicom-ctest-checks DEPENDS
    umicom-ctest-report-checks umicom-ctest-execution-checks
    umicom-ctest-suite-checks umicom-ctest-mutation-checks
    umicom-ctest-report_failures-checks umicom-ctest-fixture-helper
    umicom-ctest-discovery-checks umicom-ctest-discovery_process-checks
    umicom-ctest-discovery_native-checks)

if(BUILD_TESTING)
    add_test(NAME framework.ctest.report COMMAND umicom-ctest-report-checks)
    add_test(NAME framework.ctest.mutation COMMAND umicom-ctest-mutation-checks)
    add_test(NAME framework.ctest.report_failures COMMAND umicom-ctest-report_failures-checks)
    set(fixture_build "${CMAKE_CURRENT_BINARY_DIR}/fixture build $<CONFIG>")
    add_test(NAME framework.ctest.fixture_setup COMMAND "${CMAKE_COMMAND}"
        -S "${CMAKE_CURRENT_SOURCE_DIR}/fixture" -B "${fixture_build}"
        "-DUMICOM_CTEST_FIXTURE_HELPER=$<TARGET_FILE:umicom-ctest-fixture-helper>")
    set_tests_properties(framework.ctest.fixture_setup PROPERTIES
        FIXTURES_SETUP umicom_ctest_fixture RESOURCE_LOCK umicom_ctest_fixture TIMEOUT 60)
    set(scenarios pass literal-metacharacters literal-dot quoted-name backslash-name
        semicolon-name unicode-name long-name working-directory configuration
        wrong-configuration test-failure skip-return-code skip-expression disabled
        missing-executable missing-selection test-timeout process-timeout
        cancel-before-launch fixture-lifecycle failed-setup failed-cleanup
        large-output wrong-build-root legacy-entry aliased-input invalid-request)
    foreach(scenario IN LISTS scenarios)
        add_test(NAME "framework.ctest.execute.${scenario}"
            COMMAND umicom-ctest-execution-checks "${fixture_build}" "${scenario}")
        set_tests_properties("framework.ctest.execute.${scenario}" PROPERTIES
            FIXTURES_REQUIRED umicom_ctest_fixture RESOURCE_LOCK umicom_ctest_fixture TIMEOUT 30)
    endforeach()
    add_test(NAME framework.ctest.suite COMMAND umicom-ctest-suite-checks "${fixture_build}")
    set_tests_properties(framework.ctest.suite PROPERTIES
        FIXTURES_REQUIRED umicom_ctest_fixture RESOURCE_LOCK umicom_ctest_fixture TIMEOUT 60)
    # The existing Framework Windows process implementation uses CreateProcessA.
    # Do not advertise native Unicode argv coverage before its separate migration.
    if(WIN32)
        set_tests_properties(framework.ctest.execute.unicode-name PROPERTIES DISABLED TRUE)
    endif()
endif()

# Checked discovery is a separate test family, so it can be required before the
# larger application build. These tests compile the same production source files.
if(BUILD_TESTING)
    set(discovery_scenarios complete legacy-fragment missing-summary empty
        wrong-total duplicate-incoming duplicate-existing duplicate-summary
        row-after-summary number-overflow malformed-row duplicate-number empty-name long-name
        identity-limit embedded-nul crlf-spaces unicode ambiguous-disabled
        legacy-disabled-literal capacity-1025 capacity-exact invalid-input
        long-build-root prefix-rejection bulk-atomic)
    foreach(scenario IN LISTS discovery_scenarios)
        add_test(NAME "framework.ctest.discovery.${scenario}"
            COMMAND umicom-ctest-discovery-checks "${scenario}")
        set_tests_properties("framework.ctest.discovery.${scenario}" PROPERTIES TIMEOUT 30)
    endforeach()
    foreach(scenario success truncated cancelled timed-out launch-missing
        exit-error process-error unterminated partial-listing duplicate cancel-before)
        add_test(NAME "framework.ctest.discovery_process.${scenario}"
            COMMAND umicom-ctest-discovery_process-checks "${scenario}")
        set_tests_properties("framework.ctest.discovery_process.${scenario}" PROPERTIES TIMEOUT 30)
    endforeach()
    foreach(scenario small empty overflow capture disabled)
        set(count 2)
        set(long OFF)
        set(disabled OFF)
        if(scenario STREQUAL "empty")
            set(count 0)
        elseif(scenario STREQUAL "overflow")
            set(count 1025)
        elseif(scenario STREQUAL "capture")
            set(count 1024)
            set(long ON)
        elseif(scenario STREQUAL "disabled")
            set(disabled ON)
        endif()
        set(discovery_build "${CMAKE_CURRENT_BINARY_DIR}/discovery ${scenario} $<CONFIG>")
        add_test(NAME "framework.ctest.discovery_setup.${scenario}"
            COMMAND "${CMAKE_COMMAND}" -S "${CMAKE_CURRENT_SOURCE_DIR}/discovery_fixture"
                -B "${discovery_build}" "-DCASE_COUNT=${count}"
                "-DLONG_NAMES=${long}" "-DDISABLED_CASES=${disabled}")
        set_tests_properties("framework.ctest.discovery_setup.${scenario}" PROPERTIES
            FIXTURES_SETUP "discovery_${scenario}" TIMEOUT 60)
        add_test(NAME "framework.ctest.discovery_native.${scenario}"
            COMMAND umicom-ctest-discovery_native-checks "${discovery_build}" "${scenario}")
        set_tests_properties("framework.ctest.discovery_native.${scenario}" PROPERTIES
            FIXTURES_REQUIRED "discovery_${scenario}" TIMEOUT 60)
    endforeach()
endif()

if(BUILD_TESTING)
    # Only the fake process tests use this directory. The production guard still
    # checks a real CTest file before the injected process boundary is reached.
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build path & literal")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/build path & literal/CTestTestfile.cmake"
        "# Deliberate discovery process-failure test fixture.\n")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/unconfigured build")
    add_test(NAME framework.ctest.discovery_native.unconfigured
        COMMAND umicom-ctest-discovery_native-checks
            "${CMAKE_CURRENT_BINARY_DIR}/unconfigured build" unconfigured)
    set_tests_properties(framework.ctest.discovery_native.unconfigured PROPERTIES TIMEOUT 30)
endif()

# Scalable metadata uses the same test project and native process implementation.
include("${CMAKE_CURRENT_LIST_DIR}/json_checks.cmake")
