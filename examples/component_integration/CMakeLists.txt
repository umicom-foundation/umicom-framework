# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomComponentIntegration VERSION 1.0.0 LANGUAGES C)
# The probe deliberately models a parent application rather than only directly
# including each component after its owners. It builds canonical source subsets.
get_filename_component(FRAMEWORK "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(PROBE_REGISTER_MATRIX ON CACHE BOOL "Register integration matrix checks")
include(CTest)
add_subdirectory("${FRAMEWORK}/tests/component_integration/probe" "${CMAKE_CURRENT_BINARY_DIR}/probe")
