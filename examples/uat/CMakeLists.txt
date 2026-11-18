#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/uat/CMakeLists.txt
# PURPOSE: Build the disposable database UAT example against an installed SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomUatDatabaseExample LANGUAGES C)
find_package(UmicomFramework CONFIG REQUIRED)
find_package(SQLite3 REQUIRED)
# Prefer the current target while preserving compatibility with older CMake.
if(TARGET SQLite3::SQLite3)
    set(_uat_sqlite SQLite3::SQLite3)
else()
    set(_uat_sqlite SQLite::SQLite3)
endif()
add_executable(umicom-uat-database-example database_workflow.c)
target_link_libraries(umicom-uat-database-example PRIVATE Umicom::testing ${_uat_sqlite})
set_target_properties(umicom-uat-database-example PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES)
