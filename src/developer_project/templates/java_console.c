/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/templates/java_console.c
 *
 * PURPOSE:
 *   Generate a Java project with one JDK, runnable JAR and output contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/templates/java_console.h"
#include "console_output_recipe.h"

static const UmiDeveloperProjectTemplateFile files[] = {
    {"CMakeLists.txt",
        "#-----------------------------------------------------------------------------\n"
        "# {{APPLICATION_NAME}}\n"
        "# File: CMakeLists.txt\n"
        "#\n"
        "# PURPOSE:\n"
        "#   Build, test and stage a runnable Java archive with the selected JDK.\n"
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
        "find_package(Java REQUIRED COMPONENTS Runtime Development)\n"
        "include(UseJava)\n"
        "\n"
        "# Compile with source debug information. CMake owns generated classes\n"
        "# and resolves the same JAR path for the run, test and stage actions.\n"
        "set(CMAKE_JAVA_COMPILE_FLAGS -g -encoding UTF-8)\n"
        "add_jar(java_application SOURCES src/Main.java ENTRY_POINT Main\n"
        "    OUTPUT_NAME {{TARGET_NAME}} OUTPUT_DIR \"${CMAKE_CURRENT_BINARY_DIR}/bin\")\n"
        "add_custom_target(run-program\n"
        "    COMMAND \"${Java_JAVA_EXECUTABLE}\" -jar \"$<TARGET_PROPERTY:java_application,JAR_FILE>\"\n"
        "    DEPENDS java_application WORKING_DIRECTORY \"${CMAKE_CURRENT_SOURCE_DIR}\" VERBATIM)\n"
        "install_jar(java_application DESTINATION bin COMPONENT Runtime)\n"
        "if(BUILD_TESTING)\n"
        "    add_test(NAME {{TARGET_NAME}}.smoke COMMAND \"${CMAKE_COMMAND}\"\n"
        "        \"-DPROGRAM=${Java_JAVA_EXECUTABLE}\" \"-DEXPECTED={{APPLICATION_NAME}}\"\n"
        "        \"-DPROGRAM_ARGS=-jar;$<TARGET_PROPERTY:java_application,JAR_FILE>\"\n"
        "        -P \"${CMAKE_CURRENT_SOURCE_DIR}/tests/check_console.cmake\")\n"
        "    set_tests_properties({{TARGET_NAME}}.smoke PROPERTIES TIMEOUT 15)\n"
        "endif()\n",
        0, 0},
    {"CMakePresets.json",
        "{\n"
        "  \"version\": 6,\n"
        "  \"configurePresets\": [\n"
        "    { \"name\": \"windows-ucrt64-debug\", \"generator\": \"Ninja\", \"binaryDir\": \"${sourceDir}/build/windows-ucrt64-debug\" },\n"
        "    { \"name\": \"linux-debug\", \"generator\": \"Ninja\", \"binaryDir\": \"${sourceDir}/build/linux-debug\" }\n"
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
    {"src/Main.java",
        "/*-----------------------------------------------------------------------------\n"
        " * {{APPLICATION_NAME}}\n"
        " * File: src/Main.java\n"
        " *\n"
        " * PURPOSE:\n"
        " *   Show which application the JAR launches at its main method.\n"
        " *\n"
        " * AUTHOR AND ORGANISATION:\n"
        " * Sammy Hegab\n"
        " * Umicom Foundation\n"
        " *\n"
        " * LICENCE:\n"
        " * MIT\n"
        " *---------------------------------------------------------------------------*/\n"
        "public final class Main {\n"
        "    private Main() {}\n"
        "\n"
        "    public static void main(String[] args) {\n"
        "        System.out.println(\"{{APPLICATION_NAME}}\");\n"
        "    }\n"
        "}\n",
        0, 0},
    {"tests/check_console.cmake", umi_console_output_test, 0, 0},
    {"README.md",
        "# {{APPLICATION_NAME}}\n\n"
        "Install CMake, Ninja and a Java Development Kit (JDK) supported by your organisation. "
        "A JDK supplies the Java runtime, the javac compiler and the jar archive tool. "
        "Set JAVA_HOME to that JDK before configuring. Keep the compiler and runtime from the same installation.\n\n"
        "In PowerShell, run these commands from the generated project. Stop at the first failing command.\n\n"
        "~~~powershell\n"
        "umicom make --source . --preset windows-ucrt64-debug\n"
        "umicom build --source . --preset windows-ucrt64-debug --target run-program\n"
        "~~~\n\n"
        "The test executes the generated JAR and expects {{APPLICATION_NAME}} followed by one newline. "
        "The run target launches the same JAR. On Linux use linux-debug instead of the Windows preset. "
        "The JAR is in build/<preset>/bin/{{TARGET_NAME}}.jar. It requires a compatible Java runtime.\n\n"
        "In Studio open src/Main.java, configure and build the project, then run it. "
        "The JDK is selected by configuration. To change it, set JAVA_HOME and start a new build directory "
        "so CMake cannot reuse the old tool paths. For a local debugger, use the selected JDK's jdb. "
        "Pass -classpath and the path to the JAR, followed by Main. Enter stop in Main.main, then run.\n\n"
        "After a successful test, preview the Runtime install.\n\n"
        "~~~powershell\n"
        "umicom install --build build/windows-ucrt64-debug --prefix stage --component Runtime --dry-run\n"
        "~~~\n\n"
        "The Runtime component stages the tested JAR under bin. The JDK and server deployment are managed separately.\n",
        0, 0},
    {".gitignore", "build/\nstage/\n", 0, 0}
};

const UmiDeveloperProjectTemplate *umi_developer_project_template_java_console(void)
{
    static const UmiDeveloperProjectTemplate recipe = {
        .structure_size = sizeof(UmiDeveloperProjectTemplate),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .template_id = "developer.template.java-console",
        .title = "Java Console Application",
        .description = "JDK console application with an executable JAR, output test and Runtime stage.",
        .kind = UMI_DEVELOPER_PROJECT_META,
        .build_system = UMI_DEVELOPER_PROJECT_BUILD_CMAKE,
        .primary_language_id = "developer.language.java",
        .files = files,
        .file_count = sizeof(files) / sizeof(files[0]),
        .supports_tests = 1,
        .supports_install = 1
    };
    return &recipe;
}
