/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/templates/umicom_extension.c
 *
 * PURPOSE:
 *   Define a complete C23 community-extension starter with an inspectable
 *   manifest, stable module entry point, lifecycle callbacks, tests, install
 *   rules, and package metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/templates/umicom_extension.h"

/* Each generated file explains its role because a community contributor
 * should be able to follow the starter without reading Framework internals. */
static const UmiDeveloperProjectTemplateFile FILES[] = {
    {
        "CMakeLists.txt",
        "#-----------------------------------------------------------------------------\n"
        "# {{APPLICATION_NAME}}\n"
        "# Builds one installable Umicom extension and its descriptor test.\n"
        "# Author: Sammy Hegab\n"
        "# Organisation: Umicom Foundation\n"
        "# Licence: MIT\n"
        "#-----------------------------------------------------------------------------\n"
        "cmake_minimum_required(VERSION 3.24)\n"
        "project({{TARGET_NAME}} VERSION 0.1.0 LANGUAGES C)\n"
        "set(CMAKE_C_STANDARD 23)\n"
        "set(CMAKE_C_STANDARD_REQUIRED ON)\n"
        "set(CMAKE_C_EXTENSIONS OFF)\n\n"
        "find_package(UmicomFramework CONFIG REQUIRED COMPONENTS plugin)\n"
        "# Refuse an incomplete package instead of writing a guessed ABI into the manifest.\n"
        "if(NOT DEFINED UMICOM_FRAMEWORK_ABI_VERSION)\n"
        "  message(FATAL_ERROR \"The installed Framework does not publish its extension ABI\")\n"
        "endif()\n\n"
        "add_library({{TARGET_NAME}} MODULE src/extension.c)\n"
        "target_include_directories({{TARGET_NAME}} PUBLIC include)\n"
        "target_compile_definitions({{TARGET_NAME}} PRIVATE {{TARGET_UPPER}}_BUILDING=1)\n"
        "target_link_libraries({{TARGET_NAME}} PRIVATE Umicom::plugin)\n"
        "set_target_properties({{TARGET_NAME}} PROPERTIES PREFIX \"\")\n\n"
        "configure_file(extension.umi.in extension.umi @ONLY)\n\n"
        "include(CTest)\n"
        "if(BUILD_TESTING)\n"
        "  add_executable({{TARGET_NAME}}-descriptor-test tests/test_descriptor.c src/extension.c)\n"
        "  target_include_directories({{TARGET_NAME}}-descriptor-test PRIVATE include)\n"
        "  target_compile_definitions({{TARGET_NAME}}-descriptor-test PRIVATE {{TARGET_UPPER}}_BUILDING=1)\n"
        "  target_link_libraries({{TARGET_NAME}}-descriptor-test PRIVATE Umicom::plugin)\n"
        "  add_test(NAME {{TARGET_NAME}}.descriptor COMMAND {{TARGET_NAME}}-descriptor-test)\n"
        "endif()\n\n"
        "install(TARGETS {{TARGET_NAME}} LIBRARY DESTINATION extensions/{{TARGET_NAME}} RUNTIME DESTINATION extensions/{{TARGET_NAME}})\n"
        "install(DIRECTORY include/ DESTINATION include)\n"
        "install(FILES ${CMAKE_CURRENT_BINARY_DIR}/extension.umi DESTINATION extensions/{{TARGET_NAME}})\n"
        "set(CPACK_GENERATOR \"ZIP\")\n"
        "include(CPack)\n",
        0,
        0
    },
    {
        "CMakePresets.json",
        "{\n"
        "  \"version\": 6,\n"
        "  \"configurePresets\": [\n"
        "    {\n"
        "      \"name\": \"windows-ucrt64-debug\",\n"
        "      \"displayName\": \"Windows UCRT64 Debug\",\n"
        "      \"generator\": \"Ninja\",\n"
        "      \"binaryDir\": \"${sourceDir}/build/windows-ucrt64-debug\",\n"
        "      \"cacheVariables\": {\n"
        "        \"CMAKE_BUILD_TYPE\": \"Debug\",\n"
        "        \"CMAKE_C_COMPILER\": \"C:/msys64/ucrt64/bin/gcc.exe\",\n"
        "        \"CMAKE_MAKE_PROGRAM\": \"C:/msys64/ucrt64/bin/ninja.exe\",\n"
        "        \"BUILD_TESTING\": \"ON\"\n"
        "      }\n"
        "    }\n"
        "  ],\n"
        "  \"buildPresets\": [{\"name\": \"windows-ucrt64-debug\", \"configurePreset\": \"windows-ucrt64-debug\"}],\n"
        "  \"testPresets\": [{\"name\": \"windows-ucrt64-debug\", \"configurePreset\": \"windows-ucrt64-debug\", \"output\": {\"outputOnFailure\": true}}]\n"
        "}\n",
        0,
        0
    },
    {
        "extension.umi.in",
        "# Inspectable extension identity used before native code is loaded.\n"
        "id=org.umicom.community.{{TARGET_NAME}}\n"
        "name={{APPLICATION_NAME}}\n"
        "version=0.1.0\n"
        "abi=@UMICOM_FRAMEWORK_ABI_VERSION@\n"
        "library={{TARGET_NAME}}@CMAKE_SHARED_MODULE_SUFFIX@\n"
        "capability=extension.lifecycle\n",
        0,
        0
    },
    {
        "include/{{TARGET_NAME}}/extension.h",
        "/*-----------------------------------------------------------------------------\n"
        " * {{APPLICATION_NAME}}\n"
        " * Declares the stable entry point read by the Umicom extension host.\n"
        " * Author: Sammy Hegab\n"
        " * Organisation: Umicom Foundation\n"
        " * Licence: MIT\n"
        " *---------------------------------------------------------------------------*/\n"
        "#ifndef {{HEADER_GUARD}}\n"
        "#define {{HEADER_GUARD}}\n\n"
        "#include \"umicom/plugin/plugin.h\"\n\n"
        "#if defined(_WIN32) && defined({{TARGET_UPPER}}_BUILDING)\n"
        "#  define {{TARGET_UPPER}}_EXTENSION_API __declspec(dllexport)\n"
        "#else\n"
        "#  define {{TARGET_UPPER}}_EXTENSION_API\n"
        "#endif\n\n"
        "/** Return this extension's descriptor when the requested ABI is supported. */\n"
        "{{TARGET_UPPER}}_EXTENSION_API const UmiModuleDescriptor *\n"
        "umicom_module_query(uint32_t requested_abi);\n\n"
        "#endif\n",
        0,
        0
    },
    {
        "src/extension.c",
        "/*-----------------------------------------------------------------------------\n"
        " * {{APPLICATION_NAME}}\n"
        " * Implements a small, testable extension lifecycle.\n"
        " * Author: Sammy Hegab\n"
        " * Organisation: Umicom Foundation\n"
        " * Licence: MIT\n"
        " *---------------------------------------------------------------------------*/\n"
        "#include \"{{TARGET_NAME}}/extension.h\"\n\n"
        "/* Configuration receives Framework services but does not take ownership of them. */\n"
        "static UmiStatus extension_configure(UmiModuleContext *context)\n"
        "{\n"
        "    /* A missing context means the host cannot safely provide services. */\n"
        "    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;\n"
        "    return UMI_STATUS_OK;\n"
        "}\n\n"
        "/* Initialisation prepares extension-owned state before commands become visible. */\n"
        "static UmiStatus extension_initialise(UmiModuleContext *context)\n"
        "{\n"
        "    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;\n"
        "    return UMI_STATUS_OK;\n"
        "}\n\n"
        "/* Start is where future contributions may be registered with granted services. */\n"
        "static UmiStatus extension_start(UmiModuleContext *context)\n"
        "{\n"
        "    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;\n"
        "    return UMI_STATUS_OK;\n"
        "}\n\n"
        "/* Stop removes active work before the library can be unloaded safely. */\n"
        "static UmiStatus extension_stop(UmiModuleContext *context)\n"
        "{\n"
        "    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;\n"
        "    return UMI_STATUS_OK;\n"
        "}\n\n"
        "static const char *const PROVIDED_CAPABILITIES[] = {\n"
        "    \"extension.lifecycle\", NULL\n"
        "};\n\n"
        "static const UmiModuleDescriptor DESCRIPTOR = {\n"
        "    .structure_size = (uint32_t)sizeof(UmiModuleDescriptor),\n"
        "    .abi_version = UMICOM_FRAMEWORK_ABI_VERSION,\n"
        "    .module_id = \"org.umicom.community.{{TARGET_NAME}}\",\n"
        "    .display_name = \"{{APPLICATION_NAME}}\",\n"
        "    .module_version = {0U, 1U, 0U},\n"
        "    .kind = UMI_MODULE_UI,\n"
        "    .provided_capabilities = PROVIDED_CAPABILITIES,\n"
        "    .lifecycle = {\n"
        "        extension_configure, extension_initialise, extension_start,\n"
        "        NULL, extension_stop, NULL\n"
        "    }\n"
        "};\n\n"
        "/* The host calls this symbol before lifecycle work begins. */\n"
        "{{TARGET_UPPER}}_EXTENSION_API const UmiModuleDescriptor *\n"
        "umicom_module_query(uint32_t requested_abi)\n"
        "{\n"
        "    /* Refuse a different ABI instead of risking incompatible structure access. */\n"
        "    if (requested_abi != UMICOM_FRAMEWORK_ABI_VERSION) return NULL;\n"
        "    return &DESCRIPTOR;\n"
        "}\n",
        0,
        0
    },
    {
        "tests/test_descriptor.c",
        "/*-----------------------------------------------------------------------------\n"
        " * {{APPLICATION_NAME}} Tests\n"
        " * Verifies the extension descriptor without loading a graphical application.\n"
        " * Author: Sammy Hegab\n"
        " * Organisation: Umicom Foundation\n"
        " * Licence: MIT\n"
        " *---------------------------------------------------------------------------*/\n"
        "#include <assert.h>\n"
        "#include <string.h>\n"
        "#include \"{{TARGET_NAME}}/extension.h\"\n\n"
        "int main(void)\n"
        "{\n"
        "    const UmiModuleDescriptor *descriptor =\n"
        "        umicom_module_query(UMICOM_FRAMEWORK_ABI_VERSION);\n"
        "    assert(descriptor != NULL);\n"
        "    assert(strcmp(descriptor->module_id,\n"
        "                  \"org.umicom.community.{{TARGET_NAME}}\") == 0);\n"
        "    /* A different ABI must never receive a possibly incompatible descriptor. */\n"
        "    assert(umicom_module_query(UMICOM_FRAMEWORK_ABI_VERSION + 1U) == NULL);\n"
        "    return 0;\n"
        "}\n",
        0,
        0
    },
    {
        "README.md",
        "# {{APPLICATION_NAME}}\n\n"
        "This project is an installable Umicom extension. Its manifest is read\n"
        "before native code is loaded, and its descriptor is accepted only when\n"
        "the Framework ABI matches. Add permissions only when a feature needs\n"
        "them, keep reusable behaviour in Umicom Framework, and test every new\n"
        "contribution without opening a graphical application.\n\n"
        "Configure, build and test with the matching presets, then package the\n"
        "result with the standard package target. Review the generated manifest\n"
        "and library together before publishing them to a trusted catalogue.\n",
        0,
        0
    },
    {
        ".gitignore",
        "# Generated build, installation and package output.\n"
        "build/\ninstall/\ndist/\n*.o\n*.obj\n*.dll\n*.so\n*.dylib\n",
        0,
        0
    }
};

/* Return an immutable recipe; the generator copies and renders each file so
 * callers must not alter this shared built-in definition. */
const UmiDeveloperProjectTemplate *
umi_developer_project_template_umicom_extension(void)
{
    static const UmiDeveloperProjectTemplate project_template = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperProjectTemplate),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .template_id = "developer.template.umicom-extension",
        .title = "Umicom Extension",
        .description =
            "C23 community extension with manifest, lifecycle, tests and package rules.",
        .kind = UMI_DEVELOPER_PROJECT_EXTENSION,
        .build_system = UMI_DEVELOPER_PROJECT_BUILD_CMAKE,
        .primary_language_id = "developer.language.c23",
        .secondary_language_ids = NULL,
        .secondary_language_count = 0U,
        .recommended_frontends = 1U,
        .files = FILES,
        .file_count = sizeof(FILES) / sizeof(FILES[0]),
        .thin_application = 0,
        .supports_tests = 1,
        .supports_install = 1,
        .supports_package = 1
    };

    return &project_template;
}
