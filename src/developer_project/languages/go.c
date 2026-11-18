/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/languages/go.c
 *
 * PURPOSE:
 *   Describe Go tools without claiming that discovery validates their runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/languages/go.h"

static const char *const extensions[] = {".go"};
static const char *const tools[] = {"go"};

const UmiDeveloperProjectLanguagePack *umi_developer_project_language_go(void)
{
    /* These records declare prerequisites. PATH discovery is not an executed
     * compiler, library ABI, debugger or target-platform qualification. */
    static const UmiDeveloperProjectLanguagePack pack = {
        .structure_size = sizeof(UmiDeveloperProjectLanguagePack),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .language_id = "developer.language.go",
        .title = "Go",
        .description = "Go SDK discovery; module dependencies belong to each project.",
        .extensions = extensions, .extension_count = sizeof extensions / sizeof extensions[0],
        .build_tools = tools, .build_tool_count = sizeof tools / sizeof tools[0],
        .language_server = "", .formatter = "", .debug_adapter = "",
        .capabilities = UMI_DEVELOPER_PROJECT_LANGUAGE_BUILD | UMI_DEVELOPER_PROJECT_LANGUAGE_RUN,
        .native_language = 1, .compiled_language = 1, .built_in = 1
    };
    return &pack;
}
