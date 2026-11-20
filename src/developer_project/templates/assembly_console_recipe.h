/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/templates/assembly_console_recipe.h
 *
 * PURPOSE:
 *   Share the checked build, test and branding recipe for C and Assembly starters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_ASSEMBLY_CONSOLE_RECIPE_H
#define UMICOM_DEVELOPER_PROJECT_ASSEMBLY_CONSOLE_RECIPE_H

#include "console_output_recipe.h"

/* Both starter identities keep their original source files and output
 * directories. Test the real linked symbol using the shared console check. */
#define UMI_ASSEMBLY_CONSOLE_BUILD_RECIPE \
    "#-----------------------------------------------------------------------------\n" \
    "# {{APPLICATION_NAME}}\n" \
    "# File: CMakeLists.txt\n" \
    "#\n" \
    "# PURPOSE:\n" \
    "#   Build the C entry point and exercise the Assembly result through CTest.\n" \
    "#\n" \
    "# AUTHOR AND ORGANISATION:\n" \
    "# Sammy Hegab\n" \
    "# Umicom Foundation\n" \
    "#\n" \
    "# LICENCE:\n" \
    "# MIT\n" \
    "#-----------------------------------------------------------------------------\n" \
    "cmake_minimum_required(VERSION 3.25)\n" \
    "project({{TARGET_NAME}} VERSION 0.1.0 LANGUAGES C ASM)\n" \
    "set(CMAKE_C_STANDARD 23)\n" \
    "set(CMAKE_C_STANDARD_REQUIRED ON)\n" \
    "set(CMAKE_C_EXTENSIONS OFF)\n" \
    "set(CMAKE_RUNTIME_OUTPUT_DIRECTORY \"${CMAKE_BINARY_DIR}/bin\")\n" \
    "set(CMAKE_LIBRARY_OUTPUT_DIRECTORY \"${CMAKE_BINARY_DIR}/lib\")\n" \
    "set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY \"${CMAKE_BINARY_DIR}/lib\")\n" \
    "\n" \
    "include(CTest)\n" \
    "add_executable({{TARGET_NAME}} src/main.c src/answer.S)\n" \
    UMI_CONSOLE_OUTPUT_TEST_REGISTRATION_FOR("\"-DEXPECTED=Assembly answer: 42\"") \
    "\n" \
    "# Select the Framework source or installed package to stage the approved artwork.\n" \
    "set(UMICOM_FRAMEWORK_SOURCE_ROOT \"\" CACHE PATH \"Selected Umicom Framework checkout\")\n" \
    "if(UMICOM_FRAMEWORK_SOURCE_ROOT)\n" \
    "    include(\"${UMICOM_FRAMEWORK_SOURCE_ROOT}/cmake/UmicomApplicationBranding.cmake\")\n" \
    "elseif(UmicomFramework_DIR)\n" \
    "    find_package(UmicomFramework CONFIG REQUIRED)\n" \
    "endif()\n" \
    "if(COMMAND umicom_apply_application_branding)\n" \
    "    if(WIN32)\n" \
    "        enable_language(RC)\n" \
    "    endif()\n" \
    "    umicom_apply_application_branding(TARGET {{TARGET_NAME}}\n" \
    "        PRODUCT_NAME \"{{APPLICATION_NAME}}\" INTERNAL_NAME \"{{TARGET_NAME}}\"\n" \
    "        APPLICATION_ID \"{{APPLICATION_ID}}\"\n" \
    "        VERSION_MAJOR 0 VERSION_MINOR 1 VERSION_PATCH 0 NO_RUNTIME_DEPLOYMENT)\n" \
    "endif()\n" \
    "install(TARGETS {{TARGET_NAME}} RUNTIME DESTINATION bin)\n" \
    "set(CPACK_GENERATOR \"ZIP\")\n" \
    "include(CPack)\n"

/* An ELF program can keep a non-executable stack for this leaf function.
 * Append the note to the retained Assembly source without changing its ABI. */
#define UMI_ASSEMBLY_ELF_STACK_NOTE \
    "\n" \
    "#if defined(__ELF__)\n" \
    "    .section .note.GNU-stack,\"\",@progbits\n" \
    "#endif\n"

/* Keep the first-run instructions tied to the actual registered check. */
#define UMI_ASSEMBLY_CONSOLE_GUIDE_NOTE \
    "\n" \
    "The smoke test checks the entire program output and expects `Assembly answer: 42`.\n" \
    "If the test reports a different result, review the instruction that writes EAX in\n" \
    "`src/answer.S`, rebuild and run CTest again. This starter uses the x86-64 ABI.\n" \
    "\n" \
    "To stage the approved Umicom SVG artwork and Windows icon, add\n" \
    "`-DUMICOM_FRAMEWORK_SOURCE_ROOT=/absolute/path/to/framework` to the configure command.\n" \
    "Use `-DUmicomFramework_DIR=/absolute/path/to/installed/package` for an installed SDK.\n"

#endif
