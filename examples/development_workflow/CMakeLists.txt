#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/development_workflow/CMakeLists.txt
# PURPOSE: Build the complete Notes workflow against an installed Framework SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
project(UmicomDevelopmentWorkflowExample LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
find_package(UmicomFramework CONFIG REQUIRED)
add_executable(umicom-development-workflow-example main.c)
target_link_libraries(umicom-development-workflow-example PRIVATE Umicom::Framework)
enable_testing()
add_test(NAME notes.development_workflow COMMAND umicom-development-workflow-example complete)
set_tests_properties(notes.development_workflow PROPERTIES TIMEOUT 180)
