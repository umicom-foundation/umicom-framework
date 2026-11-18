/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/languages/freebasic.c
 *
 * PURPOSE:
 *   Describe FreeBASIC tools without claiming that discovery validates their runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/languages/freebasic.h"

static const char *const extensions[] = {".bas", ".bi"};
static const char *const tools[] = {"fbc", "cmake", "ninja"};

const UmiDeveloperProjectLanguagePack *umi_developer_project_language_freebasic(void)
{
    /* These records declare prerequisites. PATH discovery is not an executed
     * compiler, library ABI, debugger or target-platform qualification. */
    static const UmiDeveloperProjectLanguagePack pack = {
        .structure_size = sizeof(UmiDeveloperProjectLanguagePack),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .language_id = "developer.language.freebasic",
        .title = "FreeBASIC",
        .description = "FreeBASIC native console projects with the compiler runtime and include files from its distribution.",
        .extensions = extensions, .extension_count = sizeof extensions / sizeof extensions[0],
        .build_tools = tools, .build_tool_count = sizeof tools / sizeof tools[0],
        .language_server = "", .formatter = "", .debug_adapter = "",
        .capabilities = UMI_DEVELOPER_PROJECT_LANGUAGE_BUILD | UMI_DEVELOPER_PROJECT_LANGUAGE_RUN | UMI_DEVELOPER_PROJECT_LANGUAGE_TEST,
        .native_language = 1, .compiled_language = 1, .built_in = 1
    };
    return &pack;
}
