# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomSetupMaintenance LANGUAGES C)
include(CTest)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../setup_centre" "setup-centre")
