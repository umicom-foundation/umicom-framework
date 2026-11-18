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
        "${_uat_root}/src/testing/uat_report.c"
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

option(UMICOM_UAT_SQLITE_EXAMPLE "Qualify the in-memory SQLite UAT example" OFF)
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
    foreach(_mode pass assert cleanup)
        add_test(NAME framework.uat.database.${_mode} COMMAND "${CMAKE_COMMAND}"
            "-DPROGRAM=$<TARGET_FILE:umicom-uat-database-example>" "-DMODE=${_mode}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/check_database.cmake")
        set_tests_properties(framework.uat.database.${_mode} PROPERTIES TIMEOUT 15 LABELS "framework;uat;database;example")
    endforeach()
endif()
