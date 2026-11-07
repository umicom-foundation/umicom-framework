/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/languages/umicom_c.c
 *
 * PURPOSE:
 *   Describe the installed Umicom C checker and bounded preview without advertising unsupported language services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/languages/umicom_c.h"
static const char *const extensions[] = {".uc"};
static const char *const tools[] = {"umicom-c", "cmake"};
const UmiDeveloperProjectLanguagePack *umi_developer_project_language_umicom_c(void)
{
    /* The compiler is an independent executable. Framework supplies project
     * metadata and scheduling; it does not duplicate the compiler or VM. */
    static const UmiDeveloperProjectLanguagePack pack = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperProjectLanguagePack),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .language_id = "developer.language.umicom-c",
        .title = "Umicom C",
        .description = "Checked int32 arithmetic with bounded bytecode preview.",
        .extensions = extensions, .extension_count = 1U,
        .build_tools = tools, .build_tool_count = 2U,
        .language_server = "", .formatter = "", .debug_adapter = "",
        .capabilities = UMI_DEVELOPER_PROJECT_LANGUAGE_BUILD |
                        UMI_DEVELOPER_PROJECT_LANGUAGE_TEST,
        .compiled_language = 1, .built_in = 1
    };
    return &pack;
}
