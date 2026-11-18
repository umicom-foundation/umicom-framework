/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/languages/ada.c
 *
 * PURPOSE:
 *   Describe Ada tools without claiming that discovery validates their runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/languages/ada.h"

static const char *const extensions[] = {".adb", ".ads"};
static const char *const tools[] = {"gnatmake"};

const UmiDeveloperProjectLanguagePack *umi_developer_project_language_ada(void)
{
    /* These records declare prerequisites. PATH discovery is not an executed
     * compiler, library ABI, debugger or target-platform qualification. */
    static const UmiDeveloperProjectLanguagePack pack = {
        .structure_size = sizeof(UmiDeveloperProjectLanguagePack),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .language_id = "developer.language.ada",
        .title = "Ada",
        .description = "GNAT tool discovery; target runtime and GPR dependencies require project configuration.",
        .extensions = extensions, .extension_count = sizeof extensions / sizeof extensions[0],
        .build_tools = tools, .build_tool_count = sizeof tools / sizeof tools[0],
        .language_server = "", .formatter = "", .debug_adapter = "",
        .capabilities = UMI_DEVELOPER_PROJECT_LANGUAGE_BUILD | UMI_DEVELOPER_PROJECT_LANGUAGE_RUN,
        .native_language = 1, .compiled_language = 1, .built_in = 1
    };
    return &pack;
}
