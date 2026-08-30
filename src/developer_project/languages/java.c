/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/languages/java.c
 *
 * PURPOSE:
 *   Define the Framework-owned Java language-pack metadata. This is a
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
#include "umicom/developer_project/languages/java.h"

static const char *const EXTENSIONS[] = {
    ".java"
};

static const char *const BUILD_TOOLS[] = {
    "javac", "java", "mvn", "gradle"
};

const UmiDeveloperProjectLanguagePack *umi_developer_project_language_java(void)
{
    static const UmiDeveloperProjectLanguagePack pack = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperProjectLanguagePack),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .language_id = "developer.language.java",
        .title = "Java",
        .description =
            "Built-in Umicom Framework language pack for Java.",
        .extensions = EXTENSIONS,
        .extension_count = sizeof(EXTENSIONS) / sizeof(EXTENSIONS[0]),
        .build_tools = BUILD_TOOLS,
        .build_tool_count = sizeof(BUILD_TOOLS) / sizeof(BUILD_TOOLS[0]),
        .language_server = "jdtls",
        .formatter = "",
        .debug_adapter = "java-debug",
        .capabilities = UMI_DEVELOPER_PROJECT_LANGUAGE_SYNTAX | UMI_DEVELOPER_PROJECT_LANGUAGE_BUILD | UMI_DEVELOPER_PROJECT_LANGUAGE_RUN | UMI_DEVELOPER_PROJECT_LANGUAGE_DEBUG | UMI_DEVELOPER_PROJECT_LANGUAGE_TEST | UMI_DEVELOPER_PROJECT_LANGUAGE_FORMAT | UMI_DEVELOPER_PROJECT_LANGUAGE_LSP | UMI_DEVELOPER_PROJECT_LANGUAGE_PACKAGE | UMI_DEVELOPER_PROJECT_LANGUAGE_REFACTOR | UMI_DEVELOPER_PROJECT_LANGUAGE_SYMBOLS,
        .native_language = 0,
        .compiled_language = 1,
        .built_in = 1
    };

    return &pack;
}
