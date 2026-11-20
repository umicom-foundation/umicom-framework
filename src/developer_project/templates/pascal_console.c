/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/templates/pascal_console.c
 *
 * PURPOSE:
 *   Generate a compiled Free Pascal starter with run, test and staging rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/templates/pascal_console.h"

/* Studio and the CLI share the project service. Keep the compiler invocation
 * and artifact paths with the generated project so terminal builds agree. */
static const UmiDeveloperProjectTemplateFile files[] = {
    {"CMakeLists.txt",
        "#-----------------------------------------------------------------------------\n"
        "# {{APPLICATION_NAME}}\n"
        "# File: CMakeLists.txt\n"
        "#\n"
        "# PURPOSE:\n"
        "#   Build and stage the native Free Pascal console application.\n"
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
        "find_program(UMICOM_PASCAL_COMPILER NAMES fpc REQUIRED)\n"
        "\n"
        "# Keep compiler products in the build tree. The same bin path is used by\n"
        "# the generated run profile, CTest and the install component.\n"
        "set(program \"${CMAKE_CURRENT_BINARY_DIR}/bin/{{TARGET_NAME}}\")\n"
        "if(WIN32)\n"
        "    string(APPEND program \".exe\")\n"
        "endif()\n"
        "set(units \"${CMAKE_CURRENT_BINARY_DIR}/units\")\n"
        "set(source \"${CMAKE_CURRENT_SOURCE_DIR}/src/main.pas\")\n"
        "add_custom_command(OUTPUT \"${program}\"\n"
        "    COMMAND \"${CMAKE_COMMAND}\" -E make_directory \"${CMAKE_CURRENT_BINARY_DIR}/bin\" \"${units}\"\n"
        "    COMMAND \"${UMICOM_PASCAL_COMPILER}\" -g -gl \"-FU${units}\" \"-o${program}\" \"${source}\"\n"
        "    WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}\"\n"
        "    DEPENDS \"${source}\" \"${UMICOM_PASCAL_COMPILER}\"\n"
        "    VERBATIM)\n"
        "add_custom_target({{TARGET_NAME}} ALL DEPENDS \"${program}\")\n"
        "add_custom_target(run-program COMMAND \"${program}\" DEPENDS \"${program}\"\n"
        "    WORKING_DIRECTORY \"${CMAKE_CURRENT_SOURCE_DIR}\" USES_TERMINAL VERBATIM)\n"
        "install(PROGRAMS \"${program}\" DESTINATION bin COMPONENT Runtime)\n"
        "if(BUILD_TESTING)\n"
        "    add_test(NAME console.result COMMAND \"${CMAKE_COMMAND}\" \"-DPROGRAM=${program}\"\n"
        "        -P \"${CMAKE_CURRENT_SOURCE_DIR}/tests/check_result.cmake\")\n"
        "    set_tests_properties(console.result PROPERTIES TIMEOUT 10)\n"
        "endif()\n",
        0, 0},
    {"CMakePresets.json",
        "{\n"
        "  \"version\": 6,\n"
        "  \"configurePresets\": [\n"
        "    {\n"
        "      \"name\": \"windows-ucrt64-debug\",\n"
        "      \"generator\": \"Ninja\",\n"
        "      \"binaryDir\": \"${sourceDir}/build/windows-ucrt64-debug\",\n"
        "      \"cacheVariables\": { \"BUILD_TESTING\": \"ON\" }\n"
        "    },\n"
        "    {\n"
        "      \"name\": \"linux-debug\",\n"
        "      \"generator\": \"Ninja\",\n"
        "      \"binaryDir\": \"${sourceDir}/build/linux-debug\",\n"
        "      \"cacheVariables\": { \"BUILD_TESTING\": \"ON\" }\n"
        "    }\n"
        "  ],\n"
        "  \"buildPresets\": [\n"
        "    { \"name\": \"windows-ucrt64-debug\", \"configurePreset\": \"windows-ucrt64-debug\" },\n"
        "    { \"name\": \"linux-debug\", \"configurePreset\": \"linux-debug\" }\n"
        "  ],\n"
        "  \"testPresets\": [\n"
        "    { \"name\": \"windows-ucrt64-debug\", \"configurePreset\": \"windows-ucrt64-debug\", \"output\": { \"outputOnFailure\": true } },\n"
        "    { \"name\": \"linux-debug\", \"configurePreset\": \"linux-debug\", \"output\": { \"outputOnFailure\": true } }\n"
        "  ]\n"
        "}\n",
        0, 0},
    {"src/main.pas",
        "{-----------------------------------------------------------------------------\n"
        "  {{APPLICATION_NAME}}\n"
        "  File: src/main.pas\n"
        "\n"
        "  PURPOSE:\n"
        "    Demonstrate typed parameters and a native console entry point.\n"
        "\n"
        "  AUTHOR AND ORGANISATION:\n"
        "  Sammy Hegab\n"
        "  Umicom Foundation\n"
        "\n"
        "  LICENCE:\n"
        "  MIT\n"
        " -----------------------------------------------------------------------------}\n"
        "program Starter;\n"
        "{$mode objfpc}\n"
        "\n"
        "function AddNumbers(Left, Right: Integer): Integer;\n"
        "begin\n"
        "    Result := Left + Right;\n"
        "end;\n"
        "\n"
        "begin\n"
        "    WriteLn('Result: ', AddNumbers(19, 23));\n"
        "end.\n",
        0, 0},
    {"tests/check_result.cmake",
        "#-----------------------------------------------------------------------------\n"
        "# {{APPLICATION_NAME}}\n"
        "# File: tests/check_result.cmake\n"
        "#\n"
        "# PURPOSE:\n"
        "#   Compare the actual executable outcome with the expected console output.\n"
        "#\n"
        "# AUTHOR AND ORGANISATION:\n"
        "# Sammy Hegab\n"
        "# Umicom Foundation\n"
        "#\n"
        "# LICENCE:\n"
        "# MIT\n"
        "#-----------------------------------------------------------------------------\n"
        "execute_process(COMMAND \"${PROGRAM}\" RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 5)\n"
        "if(NOT \"${result}\" STREQUAL \"0\")\n"
        "    message(FATAL_ERROR \"Console program failed (${result}): ${error}\")\n"
        "endif()\n"
        "string(STRIP \"${output}\" output)\n"
        "if(NOT output STREQUAL \"Result: 42\")\n"
        "    message(FATAL_ERROR \"Expected Result: 42. Received: ${output}\")\n"
        "endif()\n",
        0, 0},
    {"README.md",
        "# {{APPLICATION_NAME}}\n"
        "\n"
        "This is a native console starter. Install CMake, Ninja and the complete Free Pascal compiler for your host, including its runtime library and linker. The recipe searches for fpc and uses the installed toolchain. It does not download its own compiler.\n"
        "\n"
        "In PowerShell, work from the generated project directory. Run each line and resolve any reported error before proceeding.\n"
        "\n"
        "~~~powershell\n"
        "Get-Command cmake, ninja, fpc\n"
        "fpc -iV\n"
        "cmake --preset windows-ucrt64-debug\n"
        "cmake --build --preset windows-ucrt64-debug\n"
        "ctest --preset windows-ucrt64-debug --no-tests=error\n"
        "& \"./build/windows-ucrt64-debug/bin/{{TARGET_NAME}}.exe\"\n"
        "~~~\n"
        "\n"
        "The executable should print Result: 42. On Linux use linux-debug for the presets and run ./build/linux-debug/bin/{{TARGET_NAME}}. The CTest case requires this output and exit code zero. Change the first operand in src/main.pas to 20, rebuild, and confirm the test reports the changed result. Restore the original operand.\n"
        "\n"
        "If CMake cannot find fpc, supply -DUMICOM_PASCAL_COMPILER=\"C:/path/to/fpc.exe\" on the configure command. A compiler still needs its matching runtime units and linker. Start with the first build error to identify a missing component.\n"
        "\n"
        "## Stage the executable\n"
        "\n"
        "After a successful build and test, stage the Runtime component in this project's local stage directory.\n"
        "\n"
        "~~~powershell\n"
        "cmake --install \"./build/windows-ucrt64-debug\" --prefix \"$PWD/stage\" --component Runtime\n"
        "~~~\n"
        "\n"
        "The staged executable is in stage/bin. Use linux-debug for a Linux build. Staging collects the host artifact. Deployment to a different operating system or architecture needs a build for that target.\n"
        "\n"
        "## Edit and debug\n"
        "\n"
        "Studio and Umicom Command use the same generated project and build settings. In Studio review the generated files before trusting the workspace, then Configure, Build, Test and Run. For debugger experiments use a compatible native debugger with the built executable and a breakpoint in src/main.pas. The build requests debug information. Debugger startup, source mapping and Pascal variable inspection depend on the installed toolchain and debugger.\n",
        0, 0},
    {".gitignore", "build/\nstage/\n", 0, 0}
};

const UmiDeveloperProjectTemplate *umi_developer_project_template_pascal_console(void)
{
    static const UmiDeveloperProjectTemplate recipe = {
        .structure_size = sizeof(UmiDeveloperProjectTemplate),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .template_id = "developer.template.pascal-console",
        .title = "Pascal Console Application",
        .description = "Native Free Pascal build, checked console output and staged runtime artifact.",
        .kind = UMI_DEVELOPER_PROJECT_EXECUTABLE,
        .build_system = UMI_DEVELOPER_PROJECT_BUILD_CMAKE,
        .primary_language_id = "developer.language.pascal",
        .files = files,
        .file_count = sizeof files / sizeof files[0],
        .supports_tests = 1,
        .supports_install = 1
    };
    return &recipe;
}
