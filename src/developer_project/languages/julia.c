/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/languages/julia.c
 *
 * PURPOSE:
 *   Describe Julia tools without claiming that discovery validates their runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/languages/julia.h"

static const char *const extensions[] = {".jl"};
static const char *const tools[] = {"julia"};

const UmiDeveloperProjectLanguagePack *umi_developer_project_language_julia(void)
{
    /* These records declare prerequisites. PATH discovery is not an executed
     * compiler, library ABI, debugger or target-platform qualification. */
    static const UmiDeveloperProjectLanguagePack pack = {
        .structure_size = sizeof(UmiDeveloperProjectLanguagePack),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .language_id = "developer.language.julia",
        .title = "Julia",
        .description = "Julia runtime discovery; package environments belong to each project.",
        .extensions = extensions, .extension_count = sizeof extensions / sizeof extensions[0],
        .build_tools = tools, .build_tool_count = sizeof tools / sizeof tools[0],
        .language_server = "", .formatter = "", .debug_adapter = "",
        .capabilities = UMI_DEVELOPER_PROJECT_LANGUAGE_RUN,
        .native_language = 0, .compiled_language = 0, .built_in = 1
    };
    return &pack;
}
