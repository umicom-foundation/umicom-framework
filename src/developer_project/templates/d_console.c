/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/templates/d_console.c
 *
 * PURPOSE:
 *   Generate incremental d build, test and run projects without downloading tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/templates/d_console.h"

static const UmiDeveloperProjectTemplateFile files[] = {
    {"CMakeLists.txt",
        "#-----------------------------------------------------------------------------\n"
        "# {{APPLICATION_NAME}}\n"
        "# File: CMakeLists.txt\n"
        "#\n"
        "# PURPOSE:\n"
        "#   Build a native D Console Application using the selected installed toolchain.\n"
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
        "find_program(UMICOM_D_COMPILER NAMES ldc2 REQUIRED)\n"
        "# A native project needs a complete compiler distribution, not only one executable.\n"
        "set(program \"${CMAKE_CURRENT_BINARY_DIR}/bin/{{TARGET_NAME}}\")\n"
        "if(WIN32)\n"
        "    string(APPEND program \".exe\")\n"
        "endif()\n"
        "file(MAKE_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}/bin\")\n"
        "add_custom_command(OUTPUT \"${program}\"\n"
        "    COMMAND \"${UMICOM_D_COMPILER}\" -g -O0 -unittest \"-of=${program}\" \"${CMAKE_CURRENT_SOURCE_DIR}/src/main.d\"\n"
        "    DEPENDS \"${CMAKE_CURRENT_SOURCE_DIR}/src/main.d\" \"${UMICOM_D_COMPILER}\"\n"
        "    VERBATIM)\n"
        "add_custom_target({{TARGET_NAME}} ALL DEPENDS \"${program}\")\n"
        "add_custom_target(run-program COMMAND \"${program}\" DEPENDS {{TARGET_NAME}}\n"
        "    WORKING_DIRECTORY \"${CMAKE_CURRENT_SOURCE_DIR}\" VERBATIM USES_TERMINAL)\n"
        "if(BUILD_TESTING)\n"
        "    add_test(NAME console.result COMMAND \"${CMAKE_COMMAND}\" \"-DPROGRAM=${program}\"\n"
        "        -P \"${CMAKE_CURRENT_SOURCE_DIR}/tests/check_result.cmake\")\n"
        "    set_tests_properties(console.result PROPERTIES TIMEOUT 10)\n"
        "endif()\n", 0, 0},
    {"CMakePresets.json",
        "{\n"
        "  \"version\": 6,\n"
        "  \"configurePresets\": [\n"
        "    {\n"
        "      \"name\": \"windows-ucrt64-debug\",\n"
        "      \"generator\": \"Ninja\",\n"
        "      \"binaryDir\": \"${sourceDir}/build/windows-ucrt64-debug\",\n"
        "      \"cacheVariables\": {\n"
        "        \"BUILD_TESTING\": \"ON\"\n"
        "      }\n"
        "    },\n"
        "    {\n"
        "      \"name\": \"linux-debug\",\n"
        "      \"generator\": \"Ninja\",\n"
        "      \"binaryDir\": \"${sourceDir}/build/linux-debug\",\n"
        "      \"cacheVariables\": {\n"
        "        \"BUILD_TESTING\": \"ON\"\n"
        "      }\n"
        "    }\n"
        "  ],\n"
        "  \"buildPresets\": [\n"
        "    {\n"
        "      \"name\": \"windows-ucrt64-debug\",\n"
        "      \"configurePreset\": \"windows-ucrt64-debug\"\n"
        "    },\n"
        "    {\n"
        "      \"name\": \"linux-debug\",\n"
        "      \"configurePreset\": \"linux-debug\"\n"
        "    }\n"
        "  ],\n"
        "  \"testPresets\": [\n"
        "    {\n"
        "      \"name\": \"windows-ucrt64-debug\",\n"
        "      \"configurePreset\": \"windows-ucrt64-debug\",\n"
        "      \"output\": {\n"
        "        \"outputOnFailure\": true\n"
        "      }\n"
        "    },\n"
        "    {\n"
        "      \"name\": \"linux-debug\",\n"
        "      \"configurePreset\": \"linux-debug\",\n"
        "      \"output\": {\n"
        "        \"outputOnFailure\": true\n"
        "      }\n"
        "    }\n"
        "  ]\n"
        "}\n", 0, 0},
    {"src/main.d",
        "/*-----------------------------------------------------------------------------\n"
        " * {{APPLICATION_NAME}}\n"
        " * File: src/main.d\n"
        " *\n"
        " * PURPOSE:\n"
        " *   Demonstrate D arithmetic, a unit test and a native console entry point.\n"
        " *\n"
        " * AUTHOR AND ORGANISATION:\n"
        " * Sammy Hegab\n"
        " * Umicom Foundation\n"
        " *\n"
        " * LICENCE:\n"
        " * MIT\n"
        " *---------------------------------------------------------------------------*/\n"
        "\n"
        "import std.stdio : writeln;\n"
        "\n"
        "int add(int left, int right) { return left + right; }\n"
        "unittest { assert(add(19, 23) == 42); }\n"
        "void main() { writeln(\"Result: \", add(19, 23)); }\n", 0, 0},
    {"tests/check_result.cmake",
        "#-----------------------------------------------------------------------------\n"
        "# {{APPLICATION_NAME}}\n"
        "# File: tests/check_result.cmake\n"
        "#\n"
        "# PURPOSE:\n"
        "#   Require both successful execution and the independently expected output.\n"
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
        "    message(FATAL_ERROR \"Program failed: ${result}\\n${error}\")\n"
        "endif()\n"
        "string(REPLACE \"\\r\\n\" \"\\n\" output \"${output}\")\n"
        "string(STRIP \"${output}\" output)\n"
        "if(NOT output STREQUAL \"Result: 42\")\n"
        "    message(FATAL_ERROR \"Expected Result: 42; received: ${output}\")\n"
        "endif()\n", 0, 0},
    {"README.md",
        "# {{APPLICATION_NAME}}\n"
        "\n"
        "## Tools and libraries\n"
        "\n"
        "Install CMake, Ninja and ldc2 before building. The compiler enables D unittest blocks. A failed assertion makes the test process fail. LDC uses druntime and Phobos: install the complete matching compiler distribution, including its supported linker and host SDK. DMD and GDC use different command-line options and cannot be substituted into this recipe. This recipe runs on the host OS. It does not produce a freestanding Kernel executable. No dependency is downloaded during configuration.\n"
        "\n"
        "## First build in PowerShell\n"
        "\n"
        "Open a terminal in this new project folder. Check discovery, then run each command separately and stop on errors:\n"
        "\n"
        "~~~powershell\n"
        "Get-Command cmake, ninja, ldc2\n"
        "cmake --preset windows-ucrt64-debug\n"
        "cmake --build --preset windows-ucrt64-debug --parallel 2\n"
        "ctest --preset windows-ucrt64-debug --output-on-failure --no-tests=error\n"
        "& \"./build/windows-ucrt64-debug/bin/{{TARGET_NAME}}.exe\"\n"
        "~~~\n"
        "\n"
        "Expected output is Result: 42. On Linux, use linux-debug for the three presets and run ./build/linux-debug/bin/{{TARGET_NAME}}. On a compatible hosted Umicom OS environment the same host tools are needed; bare-metal Umicom Kernel is a different target.\n"
        "\n"
        "## Studio workflow\n"
        "\n"
        "Select this template in New Project. Studio receives the native program path from Framework. Review Project Settings and trust your own source and build scripts. Configure discovers the compiler, Build produces the executable, Test checks its result, and Run uses the generated executable path. Changing source invalidates the build result.\n"
        "\n"
        "If discovery fails, configure with -DUMICOM_D_COMPILER=\"full/path/to/ldc2\" after checking the compiler distribution. A found executable is not proof that its linker and runtime libraries work. Read the first compiler error in Output.\n"
        "\n"
        "## Debug and extend\n"
        "\n"
        "Use a matching native debugger and debug information. For example, start gdb ./build/linux-debug/bin/{{TARGET_NAME}} in a terminal, then enter break main, run, next and quit. D runtime startup and FreeBASIC symbol naming can require a source-line breakpoint instead. Studio debugger interoperability for the chosen compiler must be qualified; compiler discovery alone does not establish it.\n"
        "\n"
        "The test expects the specific result 42, not merely exit code zero. Change one operand and check that the output test fails; restore it afterwards. Keep per-project dependencies and their licence/version records beside your source. To call Umicom Framework, use its installed C ABI and matching architecture/runtime; these minimal starters do not claim to link the full Framework SDK.\n", 0, 0},
    {".gitignore",
        "build/\n", 0, 0}
};

const UmiDeveloperProjectTemplate *umi_developer_project_template_d_console(void)
{
    static const UmiDeveloperProjectTemplate recipe = {
        .structure_size = sizeof(UmiDeveloperProjectTemplate),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .template_id = "developer.template.d-console",
        .title = "D Console Application",
        .description = "Native console build, checked output test and host executable using ldc2.",
        .kind = UMI_DEVELOPER_PROJECT_EXECUTABLE,
        .build_system = UMI_DEVELOPER_PROJECT_BUILD_CMAKE,
        .primary_language_id = "developer.language.d",
        .files = files, .file_count = sizeof files / sizeof files[0],
        .supports_tests = 1
    };
    return &recipe;
}
