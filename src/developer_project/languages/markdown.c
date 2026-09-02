/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/languages/markdown.c
 *
 * PURPOSE:
 *   Define the Framework-owned Markdown language-pack metadata. This is a
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
#include "umicom/developer_project/languages/markdown.h"

static const char *const EXTENSIONS[] = {
    ".md", ".markdown"
};

/*
 * Provide the developer project language markdown operation used by this module and its
 * client applications.
 */
const UmiDeveloperProjectLanguagePack *umi_developer_project_language_markdown(void)
{
    static const UmiDeveloperProjectLanguagePack pack = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperProjectLanguagePack),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .language_id = "developer.language.markdown",
        .title = "Markdown",
        .description =
            "Built-in Umicom Framework language pack for Markdown.",
        .extensions = EXTENSIONS,
        .extension_count = sizeof(EXTENSIONS) / sizeof(EXTENSIONS[0]),
        .build_tools = NULL,
        .build_tool_count = 0U,
        .language_server = "marksman",
        .formatter = "",
        .debug_adapter = "",
        .capabilities = UMI_DEVELOPER_PROJECT_LANGUAGE_SYNTAX | UMI_DEVELOPER_PROJECT_LANGUAGE_FORMAT | UMI_DEVELOPER_PROJECT_LANGUAGE_LSP | UMI_DEVELOPER_PROJECT_LANGUAGE_SYMBOLS,
        .native_language = 0,
        .compiled_language = 0,
        .built_in = 1
    };

    return &pack;
}
