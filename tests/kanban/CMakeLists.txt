# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomKanbanQualification LANGUAGES C)
include(CTest)
get_filename_component(_root "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
# This focused root compiles the production codec and board without GUI services.
add_library(umicom_kanban_test_base STATIC
    "${_root}/src/base/status.c" "${_root}/src/base/value_archive.c")
add_library(Umicom::base ALIAS umicom_kanban_test_base)
target_include_directories(umicom_kanban_test_base PUBLIC "${_root}/include")
set_target_properties(umicom_kanban_test_base PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
include("${_root}/cmake/UmicomKanbanSources.cmake")
add_library(umicom_kanban STATIC)
add_library(Umicom::kanban ALIAS umicom_kanban)
umicom_add_kanban_sources(umicom_kanban)
if(BUILD_TESTING)
    include("${CMAKE_CURRENT_SOURCE_DIR}/tests.cmake")
endif()
