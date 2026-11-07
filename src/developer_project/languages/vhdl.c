/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/languages/vhdl.c
 *
 * PURPOSE:
 *   Describe VHDL simulation capabilities without claiming a hardware synthesis toolchain.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/languages/vhdl.h"

static const char *const extensions[] = {".vhd", ".vhdl"};
static const char *const tools[] = {"ghdl", "cmake"};

/* Framework owns the language contract. Studio discovers it through the same
 * catalogue as other languages; simulation is performed by the project build. */
const UmiDeveloperProjectLanguagePack *umi_developer_project_language_vhdl(void)
{
    static const UmiDeveloperProjectLanguagePack pack = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperProjectLanguagePack),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .language_id = "developer.language.vhdl",
        .title = "VHDL",
        .description = "Hardware descriptions with GHDL analysis and CMake simulation tests.",
        .extensions = extensions, .extension_count = 2U,
        .build_tools = tools, .build_tool_count = 2U,
        .language_server = "", .formatter = "", .debug_adapter = "",
        .capabilities = UMI_DEVELOPER_PROJECT_LANGUAGE_BUILD |
                        UMI_DEVELOPER_PROJECT_LANGUAGE_TEST,
        .native_language = 0, .compiled_language = 1, .built_in = 1
    };
    return &pack;
}
