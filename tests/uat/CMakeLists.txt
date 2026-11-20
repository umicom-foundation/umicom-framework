#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/uat/CMakeLists.txt
#
# PURPOSE:
#   Qualify shared UAT phase orchestration using the same C sources standalone.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomUatChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_uat_root "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
    add_library(umicom-uat-check-support STATIC
        "${_uat_root}/src/testing/uat_workflow.c"
        "${_uat_root}/src/testing/uat_target.c"
        "${_uat_root}/src/testing/uat_report.c"
        "${_uat_root}/src/testing/uat_database_rows.c"
        "${_uat_root}/src/protocol/json_writer.c"
        "${_uat_root}/src/base/utf8.c"
        "${_uat_root}/src/testing/case.c"
        "${_uat_root}/src/testing/result.c"
        "${_uat_root}/src/testing/types.c"
        "${_uat_root}/src/platform/cancellation.c"
        "${_uat_root}/src/base/status.c")
    target_include_directories(umicom-uat-check-support PUBLIC "${_uat_root}/include")
    set_target_properties(umicom-uat-check-support PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES)
    set(_uat_library umicom-uat-check-support)
else()
    set(_uat_library Umicom::testing)
endif()
add_executable(umicom-uat-workflow-tests test_workflow.c)
target_link_libraries(umicom-uat-workflow-tests PRIVATE ${_uat_library})
set_target_properties(umicom-uat-workflow-tests PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-uat-workflow-tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
foreach(_case pass copy setup-fails assert-fails cleanup-fails double-failure cancel cancel-last
        cancel-before disabled bad-adapter bad-output duplicate phase-order no-assert unterminated zero-timeout)
    add_test(NAME framework.uat.workflow.${_case} COMMAND umicom-uat-workflow-tests ${_case})
    set_tests_properties(framework.uat.workflow.${_case} PROPERTIES TIMEOUT 15 LABELS "framework;uat;workflow;ownership")
endforeach()

add_executable(umicom-uat-target-tests test_target.c)
target_link_libraries(umicom-uat-target-tests PRIVATE ${_uat_library})
set_target_properties(umicom-uat-target-tests PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-uat-target-tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
foreach(_case desktop mobile web embedded detached-session detached-platform detached-target
        invalid-identity malformed-identity revision mask unknown-operation ambiguous-operation
        disallowed-operation unsupported-operation no-partial duplicate copied-scope cleanup)
    add_test(NAME framework.uat.target.${_case} COMMAND umicom-uat-target-tests ${_case})
    set_tests_properties(framework.uat.target.${_case} PROPERTIES TIMEOUT 15 LABELS "framework;uat;target;local-fixture")
endforeach()

add_executable(umicom-uat-process-arguments-tests test_process_arguments.c)
target_link_libraries(umicom-uat-process-arguments-tests PRIVATE ${_uat_library})
set_target_properties(umicom-uat-process-arguments-tests PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # The focused build uses the same process adapter and runner as Applications.
    target_sources(umicom-uat-process-arguments-tests PRIVATE
        "${_uat_root}/src/testing/uat_process.c"
        "${_uat_root}/src/testing/runner.c"
        "${_uat_root}/src/testing/suite.c"
        "${_uat_root}/src/platform/process.c")
    if(WIN32)
        target_link_libraries(umicom-uat-process-arguments-tests PRIVATE user32)
    endif()
endif()
foreach(_case quoted exact overflow open-quote fixed-field workflow)
    add_test(NAME framework.uat.process_arguments.${_case} COMMAND umicom-uat-process-arguments-tests ${_case})
    set_tests_properties(framework.uat.process_arguments.${_case} PROPERTIES TIMEOUT 20 LABELS "framework;uat;process;regression")
endforeach()

add_executable(umicom-uat-process-fixture-tests test_process_fixture.c)
target_link_libraries(umicom-uat-process-fixture-tests PRIVATE ${_uat_library})
set_target_properties(umicom-uat-process-fixture-tests PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-uat-process-fixture-tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Qualify the production adapter with the same arguments, tree lifetime and stdout owner.
    target_sources(umicom-uat-process-fixture-tests PRIVATE
        "${_uat_root}/src/testing/uat_process_fixture.c"
        "${_uat_root}/src/platform/path.c"
        "${_uat_root}/src/platform/process.c")
    if(WIN32)
        target_link_libraries(umicom-uat-process-fixture-tests PRIVATE user32)
    endif()
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-uat-process-fixture-tests)
endif()
foreach(_case pass exact-tail prefix mismatch short separate-streams child-error timeout cancel target denied
        unmapped inline deadline duplicate trusted-path argv expected-limit malformed-id
        exit-expected exit-different exit-zero-mismatch exit-stdout-mismatch exit-short-table
        exit-null-table exit-reserved-code exit-timeout dual-exact dual-mismatch dual-missing
        dual-unexpected dual-long dual-short-table dual-limit dual-null-span dual-null-table dual-code dual-timeout
        dual-raw-stress)
    set(_test "framework.uat.process_fixture.${_case}")
    add_test(NAME "${_test}" COMMAND umicom-uat-process-fixture-tests "${_case}"
        "$<TARGET_FILE:umicom-uat-process-fixture-tests>" "$<TARGET_FILE_DIR:umicom-uat-process-fixture-tests>")
    set_tests_properties("${_test}" PROPERTIES TIMEOUT 20 LABELS "framework;uat;process;local-fixture")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("${_test}" TARGETS umicom-uat-process-fixture-tests)
    endif()
endforeach()

add_executable(umicom-uat-session-policy-tests test_session_policy.c)
target_link_libraries(umicom-uat-session-policy-tests PRIVATE ${_uat_library})
set_target_properties(umicom-uat-session-policy-tests PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Compose the production registry, lock and entropy owners in this fixture.
    find_package(Threads REQUIRED)
    target_sources(umicom-uat-session-policy-tests PRIVATE
        "${_uat_root}/src/security/session.c"
        "${_uat_root}/src/security/token.c"
        "${_uat_root}/src/platform/threading.c"
        "${_uat_root}/src/platform/secure_random.c")
    target_link_libraries(umicom-uat-session-policy-tests PRIVATE Threads::Threads)
    if(WIN32)
        target_link_libraries(umicom-uat-session-policy-tests PRIVATE bcrypt)
    endif()
else()
    target_link_libraries(umicom-uat-session-policy-tests PRIVATE Umicom::security)
endif()
foreach(_case sign-out assertion-fails)
    add_test(NAME framework.uat.session_policy.${_case} COMMAND umicom-uat-session-policy-tests ${_case})
    set_tests_properties(framework.uat.session_policy.${_case} PROPERTIES TIMEOUT 20 LABELS "framework;uat;security;local-fixture")
endforeach()

option(UMICOM_UAT_SQLITE_EXAMPLE "Qualify the in-memory SQLite UAT example" OFF)
# These typed row checks run even when a host does not install SQLite.
add_executable(umicom-uat-database-rows-tests test_database_rows.c)
target_link_libraries(umicom-uat-database-rows-tests PRIVATE ${_uat_library})
set_target_properties(umicom-uat-database-rows-tests PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-uat-database-rows-tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-uat-database-rows-tests)
endif()
foreach(_case exact empty columns rows order integer real signed-zero null-empty embedded-nul blob-type
        malformed-text oversized-cell unsupported-real invalid-kind short-grid null-storage count-limit)
    add_test(NAME framework.uat.database_rows.${_case} COMMAND umicom-uat-database-rows-tests ${_case})
    set_tests_properties(framework.uat.database_rows.${_case} PROPERTIES TIMEOUT 15 LABELS "framework;uat;database;regression")
endforeach()

if(UMICOM_UAT_SQLITE_EXAMPLE)
    find_package(SQLite3 REQUIRED)
# Prefer the current target while preserving compatibility with older CMake.
if(TARGET SQLite3::SQLite3)
    set(_uat_sqlite SQLite3::SQLite3)
else()
    set(_uat_sqlite SQLite::SQLite3)
endif()
    add_executable(umicom-uat-database-example "${CMAKE_CURRENT_SOURCE_DIR}/../../examples/uat/database_workflow.c")
    target_link_libraries(umicom-uat-database-example PRIVATE ${_uat_library} ${_uat_sqlite})
    set_target_properties(umicom-uat-database-example PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES)
    # The transaction case is opt-in. Keep the fixed four-step fixture cases.
    foreach(_mode pass assert cleanup rollback typed typed-rejected)
        add_test(NAME framework.uat.database.${_mode} COMMAND "${CMAKE_COMMAND}"
            "-DPROGRAM=$<TARGET_FILE:umicom-uat-database-example>" "-DMODE=${_mode}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/check_database.cmake")
        set_tests_properties(framework.uat.database.${_mode} PROPERTIES TIMEOUT 15 LABELS "framework;uat;database;example")
    endforeach()
endif()
