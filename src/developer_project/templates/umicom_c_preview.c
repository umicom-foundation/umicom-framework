/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/templates/umicom_c_preview.c
 *
 * PURPOSE:
 *   Generate a checked arithmetic project using the independent Umicom C compiler instead of copying its implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/templates/umicom_c_preview.h"

static const UmiDeveloperProjectTemplateFile files[] = {
    {"CMakeLists.txt",
        "#-----------------------------------------------------------------------------\n"
        "# {{APPLICATION_NAME}}\n"
        "# File: CMakeLists.txt\n"
        "#\n"
        "# PURPOSE:\n"
        "#   Check changed source with the installed Umicom C tool and expose bounded preview tests.\n"
        "#\n"
        "# AUTHOR AND ORGANISATION:\n"
        "# Sammy Hegab\n"
        "# Umicom Foundation\n"
        "#\n"
        "# LICENCE:\n"
        "# MIT\n"
        "#-----------------------------------------------------------------------------\n"
        "cmake_minimum_required(VERSION 3.26)\n"
        "project({{TARGET_NAME}} LANGUAGES NONE)\n"
        "include(CTest)\n"
        "find_program(UMICOM_C_EXECUTABLE NAMES umicom-c REQUIRED)\n"
        "# Source or tool changes invalidate the build-tree success marker.\n"
        "add_custom_command(OUTPUT \"${CMAKE_CURRENT_BINARY_DIR}/source.checked\"\n"
        "    COMMAND \"${UMICOM_C_EXECUTABLE}\" check \"${CMAKE_CURRENT_SOURCE_DIR}/src/main.uc\"\n"
        "    COMMAND \"${CMAKE_COMMAND}\" -E touch \"${CMAKE_CURRENT_BINARY_DIR}/source.checked\"\n"
        "    DEPENDS \"${CMAKE_CURRENT_SOURCE_DIR}/src/main.uc\" \"${UMICOM_C_EXECUTABLE}\"\n"
        "    VERBATIM)\n"
        "add_custom_target(check-source ALL DEPENDS \"${CMAKE_CURRENT_BINARY_DIR}/source.checked\")\n"
        "add_custom_target(preview\n"
        "    COMMAND \"${UMICOM_C_EXECUTABLE}\" run \"${CMAKE_CURRENT_SOURCE_DIR}/src/main.uc\" --max-steps 256\n"
        "    DEPENDS check-source VERBATIM)\n"
        "# Inspection reports structural resource bounds without evaluating arithmetic.\n"
        "add_custom_target(inspect-program\n"
        "    COMMAND \"${UMICOM_C_EXECUTABLE}\" inspect \"${CMAKE_CURRENT_SOURCE_DIR}/src/main.uc\"\n"
        "    DEPENDS check-source VERBATIM)\n"
        "if(BUILD_TESTING)\n"
        "    foreach(case IN ITEMS result syntax division fuel)\n"
        "        add_test(NAME preview.${case} COMMAND \"${CMAKE_COMMAND}\"\n"
        "            \"-DTOOL=${UMICOM_C_EXECUTABLE}\" \"-DROOT=${CMAKE_CURRENT_SOURCE_DIR}\"\n"
        "            \"-DCASE=${case}\" -P \"${CMAKE_CURRENT_SOURCE_DIR}/tests/check_preview.cmake\")\n"
        "        set_tests_properties(preview.${case} PROPERTIES TIMEOUT 10)\n"
        "    endforeach()\n"
        "endif()\n", 0, 0},
    {"CMakePresets.json",
        "{\n"
        "  \"version\": 6,\n"
        "  \"configurePresets\": [\n"
        "    {\n"
        "      \"name\": \"preview\",\n"
        "      \"displayName\": \"Umicom C arithmetic preview\",\n"
        "      \"generator\": \"Ninja\",\n"
        "      \"binaryDir\": \"${sourceDir}/build/preview\",\n"
        "      \"cacheVariables\": {\n"
        "        \"BUILD_TESTING\": \"ON\"\n"
        "      }\n"
        "    }\n"
        "  ],\n"
        "  \"buildPresets\": [\n"
        "    {\n"
        "      \"name\": \"preview\",\n"
        "      \"configurePreset\": \"preview\"\n"
        "    }\n"
        "  ],\n"
        "  \"testPresets\": [\n"
        "    {\n"
        "      \"name\": \"preview\",\n"
        "      \"configurePreset\": \"preview\",\n"
        "      \"output\": {\n"
        "        \"outputOnFailure\": true\n"
        "      }\n"
        "    }\n"
        "  ]\n"
        "}\n", 0, 0},
    {"src/main.uc",
        "/*-----------------------------------------------------------------------------\n"
        " * {{APPLICATION_NAME}}\n"
        " * File: src/main.uc\n"
        " *\n"
        " * PURPOSE:\n"
        " *   Evaluate checked arithmetic in the Umicom C preview runtime.\n"
        " *\n"
        " * AUTHOR AND ORGANISATION:\n"
        " * Sammy Hegab\n"
        " * Umicom Foundation\n"
        " *\n"
        " * LICENCE:\n"
        " * MIT\n"
        " *---------------------------------------------------------------------------*/\n"
        "int main(void)\n"
        "{\n"
        "    return 6 * 7;\n"
        "}\n", 0, 0},
    {"tests/invalid.uc",
        "/*-----------------------------------------------------------------------------\n"
        " * {{APPLICATION_NAME}}\n"
        " * File: tests/invalid.uc\n"
        " *\n"
        " * PURPOSE:\n"
        " *   Supply deliberately incomplete arithmetic to verify source rejection.\n"
        " *\n"
        " * AUTHOR AND ORGANISATION:\n"
        " * Sammy Hegab\n"
        " * Umicom Foundation\n"
        " *\n"
        " * LICENCE:\n"
        " * MIT\n"
        " *---------------------------------------------------------------------------*/\n"
        "int main(void) { return 6 + ; }\n", 0, 0},
    {"tests/division.uc",
        "/*-----------------------------------------------------------------------------\n"
        " * {{APPLICATION_NAME}}\n"
        " * File: tests/division.uc\n"
        " *\n"
        " * PURPOSE:\n"
        " *   Exercise a runtime division-by-zero rejection without invoking native undefined behaviour.\n"
        " *\n"
        " * AUTHOR AND ORGANISATION:\n"
        " * Sammy Hegab\n"
        " * Umicom Foundation\n"
        " *\n"
        " * LICENCE:\n"
        " * MIT\n"
        " *---------------------------------------------------------------------------*/\n"
        "int main(void) { return 9 / 0; }\n", 0, 0},
    {"tests/check_preview.cmake",
        "#-----------------------------------------------------------------------------\n"
        "# {{APPLICATION_NAME}}\n"
        "# File: tests/check_preview.cmake\n"
        "#\n"
        "# PURPOSE:\n"
        "#   Check real compiler exit codes and values; tool launch failures must not pass negative cases.\n"
        "#\n"
        "# AUTHOR AND ORGANISATION:\n"
        "# Sammy Hegab\n"
        "# Umicom Foundation\n"
        "#\n"
        "# LICENCE:\n"
        "# MIT\n"
        "#-----------------------------------------------------------------------------\n"
        "if(NOT DEFINED TOOL OR NOT DEFINED ROOT OR NOT DEFINED CASE)\n"
        "    message(FATAL_ERROR \"TOOL, ROOT and CASE are required\")\n"
        "endif()\n"
        "set(source \"${ROOT}/src/main.uc\")\n"
        "set(command run)\n"
        "set(expected_exit 1)\n"
        "set(options)\n"
        "if(CASE STREQUAL \"result\")\n"
        "    set(expected_exit 0)\n"
        "    set(options --max-steps 256)\n"
        "elseif(CASE STREQUAL \"syntax\")\n"
        "    set(source \"${ROOT}/tests/invalid.uc\")\n"
        "    set(command check)\n"
        "elseif(CASE STREQUAL \"division\")\n"
        "    set(source \"${ROOT}/tests/division.uc\")\n"
        "elseif(CASE STREQUAL \"fuel\")\n"
        "    set(options --max-steps 1)\n"
        "else()\n"
        "    message(FATAL_ERROR \"Unknown preview case\")\n"
        "endif()\n"
        "execute_process(COMMAND \"${TOOL}\" \"${command}\" \"${source}\" ${options}\n"
        "    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 5)\n"
        "# Timeout, launch and usage failures are not accepted compiler diagnostics.\n"
        "if(NOT \"${result}\" STREQUAL \"${expected_exit}\")\n"
        "    message(FATAL_ERROR \"Preview ${CASE}: exit ${result}; expected ${expected_exit}\\n${output}\\n${error}\")\n"
        "endif()\n"
        "if(CASE STREQUAL \"result\")\n"
        "    string(REPLACE \"\\r\\n\" \"\\n\" output \"${output}\")\n"
        "    string(FIND \"${output}\" \"Result: 42\\n\" found)\n"
        "    if(found EQUAL -1)\n"
        "        message(FATAL_ERROR \"Preview returned an unexpected value: ${output}\")\n"
        "    endif()\n"
        "elseif(error STREQUAL \"\")\n"
        "    message(FATAL_ERROR \"Rejected input did not explain its failure\")\n"
        "endif()\n", 0, 0},
    {"README.md",
        "# {{APPLICATION_NAME}}\n"
        "\n"
        "This project uses the **Umicom C arithmetic profile**. It accepts a single function returning checked signed 32-bit arithmetic. The separate Umicom C executable supplies the compiler and bounded virtual machine. The profile does not implement the complete C23 language.\n"
        "\n"
        "## Prepare the tools\n"
        "\n"
        "Install CMake, Ninja and Umicom C. Put their executable directories on PATH. In PowerShell, verify them with Get-Command cmake, ninja, umicom-c. In Bash use command -v for each name. Run umicom-c --version to see the installed profile.\n"
        "\n"
        "From this project's folder, run each command separately:\n"
        "\n"
        "~~~text\n"
        "cmake --preset preview\n"
        "cmake --build --preset preview\n"
        "ctest --preset preview --output-on-failure --no-tests=error\n"
        "cmake --build build/preview --target preview\n"
        "~~~\n"
        "\n"
        "The last command should print Result: 42 and an instruction count. Tests require that value and exercise syntax rejection, division by zero and insufficient execution fuel. After intentionally changing the example's result, update the expected value in tests/check_preview.cmake only after checking the arithmetic yourself.\n"
        "\n"
        "If discovery fails, configure with -DUMICOM_C_EXECUTABLE= followed immediately by the full executable path; quote paths containing spaces. This project needs no native C compiler. Building Umicom C itself requires its documented native toolchain.\n"
        "\n"
        "## Open in Studio\n"
        "\n"
        "Create the **Umicom C Arithmetic Preview** template or open its folder. Select the preview configure/build/test preset. Configure finds the tool, Build checks changed source, and Test runs the bounded examples. Build the preview target from a terminal or custom target selection to evaluate the source. Studio's ordinary native Run and Debug commands do not launch this virtual-machine profile.\n"
        "\n"
        "A file is a named collection of bytes. src/main.uc contains the program. The compiler recognises its text, checks its structure and creates bytecode in memory. The runtime evaluates instructions with an explicit maximum step count. A source check does not prove evaluation will succeed: 9 / 0 is syntactically valid but fails during execution.\n"
        "\n"
        "## Inspect before evaluating\n"
        "\n"
        "With an Umicom C installation that supports the inspect command, run cmake --build build/preview --target inspect-program. This target reports instruction count, peak stack values, constants, arithmetic operations and encoded byte count. For the unchanged example expect 4 instructions, 2 peak stack values, 2 constants, 1 arithmetic operation and 44 encoded bytes. Inspection does not execute arithmetic. Division by zero and integer overflow still require execution checks. If the command is unknown, update Umicom C; the ordinary check and preview targets remain available.\n"
        "\n"
        "## Preserve your work\n"
        "\n"
        "Checking writes only build/preview/source.checked after success. Editing source or replacing the compiler invalidates this marker. Preview evaluates source without saving a bytecode artifact. To examine one, use umicom-c compile with src/main.uc and a new output filename, then umicom-c disassemble with that filename. Compilation refuses to overwrite an existing artifact.\n"
        "\n"
        "The runtime's bounds describe this arithmetic profile. They do not establish safe execution for arbitrary native programs or third-party tools.\n", 0, 0},
    {".gitignore",
        "build/\n"
        "*.umcb\n", 0, 0}
};

const UmiDeveloperProjectTemplate *umi_developer_project_template_umicom_c_preview(void)
{
    static const UmiDeveloperProjectTemplate recipe = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperProjectTemplate),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .template_id = "developer.template.umicom-c-preview",
        .title = "Umicom C Arithmetic Preview",
        .description = "Incremental source checks and bounded arithmetic tests with the installed Umicom C tool.",
        .kind = UMI_DEVELOPER_PROJECT_META,
        .build_system = UMI_DEVELOPER_PROJECT_BUILD_CMAKE,
        .primary_language_id = "developer.language.umicom-c",
        .files = files, .file_count = sizeof(files) / sizeof(files[0]),
        .supports_tests = 1
    };
    return &recipe;
}
