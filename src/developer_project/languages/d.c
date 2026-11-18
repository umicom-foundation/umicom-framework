/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/languages/d.c
 *
 * PURPOSE:
 *   Describe D tools without claiming that discovery validates their runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/languages/d.h"

static const char *const extensions[] = {".d", ".di"};
static const char *const tools[] = {"ldc2", "cmake", "ninja"};

const UmiDeveloperProjectLanguagePack *umi_developer_project_language_d(void)
{
    /* These records declare prerequisites. PATH discovery is not an executed
     * compiler, library ABI, debugger or target-platform qualification. */
    static const UmiDeveloperProjectLanguagePack pack = {
        .structure_size = sizeof(UmiDeveloperProjectLanguagePack),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .language_id = "developer.language.d",
        .title = "D",
        .description = "LDC builds native D programs; druntime and Phobos come from the selected LDC distribution.",
        .extensions = extensions, .extension_count = sizeof extensions / sizeof extensions[0],
        .build_tools = tools, .build_tool_count = sizeof tools / sizeof tools[0],
        .language_server = "", .formatter = "", .debug_adapter = "",
        .capabilities = UMI_DEVELOPER_PROJECT_LANGUAGE_BUILD | UMI_DEVELOPER_PROJECT_LANGUAGE_RUN | UMI_DEVELOPER_PROJECT_LANGUAGE_TEST,
        .native_language = 1, .compiled_language = 1, .built_in = 1
    };
    return &pack;
}
