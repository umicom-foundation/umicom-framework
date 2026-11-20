#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/uat_browser/CMakeLists.txt
# PURPOSE: Build the Chromium observation exercise against the testing SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomUatBrowserObservation LANGUAGES C)
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS testing)
add_executable(umicom-uat-browser-observation main.c)
target_link_libraries(umicom-uat-browser-observation PRIVATE Umicom::testing)
set_target_properties(umicom-uat-browser-observation PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
