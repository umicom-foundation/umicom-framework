#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/document_preview/CMakeLists.txt
# PURPOSE: Build the local-document reader using the installed Framework SDK.
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomDocumentPreview LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
find_package(UmicomFramework CONFIG REQUIRED)
add_executable(umicom-document-preview main.c)
target_link_libraries(umicom-document-preview PRIVATE Umicom::browser)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-document-preview PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
include(CTest)
if(BUILD_TESTING)
    add_test(NAME document_preview.example
        COMMAND umicom-document-preview "${CMAKE_CURRENT_SOURCE_DIR}/lesson.html")
    set_tests_properties(document_preview.example PROPERTIES
        TIMEOUT 15 PASS_REGULAR_EXPRESSION "Title: Learning with Umicom")
    add_test(NAME document_preview.usage COMMAND umicom-document-preview)
    set_tests_properties(document_preview.usage PROPERTIES TIMEOUT 15 WILL_FAIL TRUE)
endif()
