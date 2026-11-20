/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/templates/console_output_recipe.h
 *
 * PURPOSE:
 *   Share a generated console test that executes the application and checks its complete output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_PROJECT_CONSOLE_OUTPUT_RECIPE_H
#define UMICOM_DEVELOPER_PROJECT_CONSOLE_OUTPUT_RECIPE_H

/* Native console projects resolve the built executable from the same CMake
 * target they install. Running the test must exercise that exact output. */
#define UMI_CONSOLE_OUTPUT_TEST_REGISTRATION_FOR(expectation) \
    "if(BUILD_TESTING)\n" \
    "    add_test(NAME {{TARGET_NAME}}.smoke COMMAND \"${CMAKE_COMMAND}\"\n" \
    "        \"-DPROGRAM=$<TARGET_FILE:{{TARGET_NAME}}>\" " expectation "\n" \
    "        -P \"${CMAKE_CURRENT_SOURCE_DIR}/tests/check_console.cmake\")\n" \
    "    set_tests_properties({{TARGET_NAME}}.smoke PROPERTIES TIMEOUT 15)\n" \
    "endif()\n"

#define UMI_CONSOLE_OUTPUT_TEST_REGISTRATION \
    UMI_CONSOLE_OUTPUT_TEST_REGISTRATION_FOR("\"-DEXPECTED={{APPLICATION_NAME}}\"")

static const char umi_console_output_test[] =
    "#-----------------------------------------------------------------------------\n"
    "# {{APPLICATION_NAME}}\n"
    "# File: tests/check_console.cmake\n"
    "#\n"
    "# PURPOSE:\n"
    "#   Launch the generated console program and check its complete output.\n"
    "#\n"
    "# AUTHOR AND ORGANISATION:\n"
    "# Sammy Hegab\n"
    "# Umicom Foundation\n"
    "#\n"
    "# LICENCE:\n"
    "# MIT\n"
    "#-----------------------------------------------------------------------------\n"
    /* Preserve the inline-only guard for review. Both input routes now
     * resolve to the same text comparison before the program is launched. */
#if 0
    "if(NOT DEFINED PROGRAM OR NOT DEFINED EXPECTED OR NOT IS_ABSOLUTE \"${PROGRAM}\")\n"
    "    message(FATAL_ERROR \"The test requires the built console path and expected name.\")\n"
    "endif()\n"
#else
    "if(NOT DEFINED PROGRAM OR NOT IS_ABSOLUTE \"${PROGRAM}\")\n"
    "    message(FATAL_ERROR \"The test requires the absolute path to the built console.\")\n"
    "endif()\n"
    "# Read the saved expectation on every test run so editing it needs no rebuild.\n"
    "if(DEFINED EXPECTED_FILE)\n"
    "    if(DEFINED EXPECTED OR NOT IS_ABSOLUTE \"${EXPECTED_FILE}\"\n"
    "       OR NOT EXISTS \"${EXPECTED_FILE}\" OR IS_DIRECTORY \"${EXPECTED_FILE}\")\n"
    "        message(FATAL_ERROR \"Select one expectation using an absolute path to a readable text file.\")\n"
    "    endif()\n"
    "    file(READ \"${EXPECTED_FILE}\" expected)\n"
    "    string(REPLACE \"\\r\\n\" \"\\n\" expected \"${expected}\")\n"
    "elseif(DEFINED EXPECTED)\n"
    "    set(expected \"${EXPECTED}\\n\")\n"
    "else()\n"
    "    message(FATAL_ERROR \"The test requires an output expectation.\")\n"
    "endif()\n"
#endif
    "# Native console programs use an empty argument list. Managed runtimes can select a built artifact.\n"
    "execute_process(COMMAND \"${PROGRAM}\" ${PROGRAM_ARGS} RESULT_VARIABLE status\n"
    "    OUTPUT_VARIABLE output ERROR_VARIABLE diagnostic TIMEOUT 10)\n"
    "string(REPLACE \"\\r\\n\" \"\\n\" output \"${output}\")\n"
    /* Keep the inline comparison for review. Reading the two inputs into
     * one expected value preserves exact output and diagnostic checks. */
#if 0
    "if(NOT status STREQUAL \"0\" OR NOT output STREQUAL \"${EXPECTED}\\n\" OR NOT diagnostic STREQUAL \"\")\n"
    "    message(FATAL_ERROR \"Console output or exit status differed (exit ${status}).\\nExpected: ${EXPECTED}\\nActual: ${output}\\n${diagnostic}\")\n"
#else
    "if(NOT status STREQUAL \"0\" OR NOT output STREQUAL expected OR NOT diagnostic STREQUAL \"\")\n"
    "    message(FATAL_ERROR \"Console output or exit status differed (exit ${status}).\\nExpected:\\n${expected}\\nActual:\\n${output}\\n${diagnostic}\")\n"
#endif
    "endif()\n";

#endif
