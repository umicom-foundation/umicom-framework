/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/languages/cmake.c
 *
 * PURPOSE:
 *   Define the Framework-owned CMake language-pack metadata. This is a
 *   capability declaration; execution remains behind toolchain/language-provider
 *   contracts rather than being hard-coded into Umicom Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/languages/cmake.h"

static const char *const EXTENSIONS[] = {
    "CMakeLists.txt", ".cmake"
};

static const char *const BUILD_TOOLS[] = {
    "cmake", "ninja"
};

const UmiDeveloperProjectLanguagePack *umi_developer_project_language_cmake(void)
{
    static const UmiDeveloperProjectLanguagePack pack = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperProjectLanguagePack),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .language_id = "developer.language.cmake",
        .title = "CMake",
        .description =
            "Built-in Umicom Framework language pack for CMake.",
        .extensions = EXTENSIONS,
        .extension_count = sizeof(EXTENSIONS) / sizeof(EXTENSIONS[0]),
        .build_tools = BUILD_TOOLS,
        .build_tool_count = sizeof(BUILD_TOOLS) / sizeof(BUILD_TOOLS[0]),
        .language_server = "cmake-language-server",
        .formatter = "cmake-format",
        .debug_adapter = "",
        .capabilities = UMI_DEVELOPER_PROJECT_LANGUAGE_SYNTAX | UMI_DEVELOPER_PROJECT_LANGUAGE_BUILD | UMI_DEVELOPER_PROJECT_LANGUAGE_FORMAT | UMI_DEVELOPER_PROJECT_LANGUAGE_LSP | UMI_DEVELOPER_PROJECT_LANGUAGE_SYMBOLS,
        .native_language = 0,
        .compiled_language = 0,
        .built_in = 1
    };

    return &pack;
}
