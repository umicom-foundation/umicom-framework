#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/development/c_assembly_notes/CMakeLists.txt
# PURPOSE: Build, test, install and package a C/Assembly Notes application.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.21)
project(UmicomNotes LANGUAGES C ASM VERSION 1.0.0)
include(CTest)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8 OR
   NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$" OR
   NOT CMAKE_C_COMPILER_ID MATCHES "GNU|Clang" OR APPLE)
    message(FATAL_ERROR "This assembly lesson requires x86-64 Windows/UCRT64 or Linux and GCC/Clang with the GNU assembler syntax.")
endif()
add_library(umicom-notes-core STATIC notes.c count_byte.S)
target_include_directories(umicom-notes-core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(umicom-notes main.c)
target_link_libraries(umicom-notes PRIVATE umicom-notes-core)
foreach(target umicom-notes-core umicom-notes)
    target_compile_options(${target} PRIVATE
        $<$<COMPILE_LANGUAGE:C>:-Wall;-Wextra;-Wpedantic;-Wconversion;-Wshadow;-Werror>)
endforeach()
if(BUILD_TESTING)
    add_executable(umicom-notes-checks test_notes.c)
    target_link_libraries(umicom-notes-checks PRIVATE umicom-notes-core)
    target_compile_options(umicom-notes-checks PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    foreach(scenario empty single-line lines unicode binary reference invalid)
        add_test(NAME "notes.statistics.${scenario}" COMMAND umicom-notes-checks "${scenario}")
    endforeach()

endif()
install(TARGETS umicom-notes RUNTIME DESTINATION bin)
install(FILES sample.notes DESTINATION share/umicom-notes)
set(CPACK_PACKAGE_NAME "Umicom-Notes")
set(CPACK_PACKAGE_VENDOR "Umicom Foundation")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_GENERATOR ZIP)
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_PACKAGE_FILE_NAME "Umicom-Notes")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/packages")
include(CPack)
