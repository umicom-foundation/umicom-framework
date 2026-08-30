/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/languages/cpython.c
 *
 * PURPOSE:
 *   Define the Framework-owned CPython language-pack metadata. This is a
 *   capability declaration; execution remains behind toolchain/language-provider
 *   contracts rather than being hard-coded into Umicom Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/languages/cpython.h"

static const char *const EXTENSIONS[] = {
    ".py", ".pyi"
};

static const char *const BUILD_TOOLS[] = {
    "python", "python3"
};

const UmiDeveloperProjectLanguagePack *umi_developer_project_language_cpython(void)
{
    static const UmiDeveloperProjectLanguagePack pack = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperProjectLanguagePack),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .language_id = "developer.language.cpython",
        .title = "CPython",
        .description =
            "Built-in Umicom Framework language pack for CPython.",
        .extensions = EXTENSIONS,
        .extension_count = sizeof(EXTENSIONS) / sizeof(EXTENSIONS[0]),
        .build_tools = BUILD_TOOLS,
        .build_tool_count = sizeof(BUILD_TOOLS) / sizeof(BUILD_TOOLS[0]),
        .language_server = "pyright",
        .formatter = "black",
        .debug_adapter = "debugpy",
        .capabilities = UMI_DEVELOPER_PROJECT_LANGUAGE_SYNTAX | UMI_DEVELOPER_PROJECT_LANGUAGE_RUN | UMI_DEVELOPER_PROJECT_LANGUAGE_DEBUG | UMI_DEVELOPER_PROJECT_LANGUAGE_TEST | UMI_DEVELOPER_PROJECT_LANGUAGE_FORMAT | UMI_DEVELOPER_PROJECT_LANGUAGE_LSP | UMI_DEVELOPER_PROJECT_LANGUAGE_PACKAGE | UMI_DEVELOPER_PROJECT_LANGUAGE_REFACTOR | UMI_DEVELOPER_PROJECT_LANGUAGE_SYMBOLS,
        .native_language = 0,
        .compiled_language = 0,
        .built_in = 1
    };

    return &pack;
}
