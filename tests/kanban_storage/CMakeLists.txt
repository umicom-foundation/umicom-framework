# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomKanbanStorageQualification LANGUAGES C)
include(CTest)
get_filename_component(_root "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
# Build existing board qualification as well as the real native storage adapter.
add_subdirectory("${_root}/tests/kanban" "kanban-core")
add_library(umi_kanban_native_files STATIC
    "${_root}/src/platform/path.c"
    "${_root}/src/platform/output_file.c"
    "${_root}/src/platform/input_file.c"
    "${_root}/src/platform/file_lease.c"
    "${_root}/src/platform/rooted_files.c")
add_library(Umicom::platform ALIAS umi_kanban_native_files)
target_include_directories(umi_kanban_native_files PUBLIC "${_root}/include")
target_link_libraries(umi_kanban_native_files PUBLIC Umicom::base)
set_target_properties(umi_kanban_native_files PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
include("${_root}/cmake/UmicomKanbanStorageSources.cmake")
add_library(umicom_kanban_storage STATIC)
add_library(Umicom::kanban_storage ALIAS umicom_kanban_storage)
umicom_add_kanban_storage_sources(umicom_kanban_storage)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom_kanban_storage PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    include("${CMAKE_CURRENT_SOURCE_DIR}/tests.cmake")
endif()
