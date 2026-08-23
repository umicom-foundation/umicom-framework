/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/languages/php.c
 *
 * PURPOSE:
 *   Define the Framework-owned PHP language-pack metadata. This is a
 *   capability declaration; execution remains behind toolchain/language-provider
 *   contracts rather than being hard-coded into Umicom Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/languages/php.h"

static const char *const EXTENSIONS[] = {
    ".php"
};

static const char *const BUILD_TOOLS[] = {
    "php", "composer"
};

const UmiDeveloperProjectLanguagePack *umi_developer_project_language_php(void)
{
    static const UmiDeveloperProjectLanguagePack pack = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperProjectLanguagePack),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .language_id = "developer.language.php",
        .title = "PHP",
        .description =
            "Built-in Umicom Framework language pack for PHP.",
        .extensions = EXTENSIONS,
        .extension_count = sizeof(EXTENSIONS) / sizeof(EXTENSIONS[0]),
        .build_tools = BUILD_TOOLS,
        .build_tool_count = sizeof(BUILD_TOOLS) / sizeof(BUILD_TOOLS[0]),
        .language_server = "intelephense",
        .formatter = "",
        .debug_adapter = "xdebug",
        .capabilities = UMI_DEVELOPER_PROJECT_LANGUAGE_SYNTAX | UMI_DEVELOPER_PROJECT_LANGUAGE_RUN | UMI_DEVELOPER_PROJECT_LANGUAGE_DEBUG | UMI_DEVELOPER_PROJECT_LANGUAGE_TEST | UMI_DEVELOPER_PROJECT_LANGUAGE_FORMAT | UMI_DEVELOPER_PROJECT_LANGUAGE_LSP | UMI_DEVELOPER_PROJECT_LANGUAGE_PACKAGE | UMI_DEVELOPER_PROJECT_LANGUAGE_REFACTOR | UMI_DEVELOPER_PROJECT_LANGUAGE_SYMBOLS,
        .native_language = 0,
        .compiled_language = 0,
        .built_in = 1
    };

    return &pack;
}
