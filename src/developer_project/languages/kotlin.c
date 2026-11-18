/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/languages/kotlin.c
 *
 * PURPOSE:
 *   Describe Kotlin tools without claiming that discovery validates their runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/languages/kotlin.h"

static const char *const extensions[] = {".kt", ".kts"};
#ifdef _WIN32
/* Discovery records the supplied launcher file; executing a script requires its explicit host. */
static const char *const tools[] = {"kotlinc.bat", "java"};
#else
static const char *const tools[] = {"kotlinc", "java"};
#endif

const UmiDeveloperProjectLanguagePack *umi_developer_project_language_kotlin(void)
{
    /* These records declare prerequisites. PATH discovery is not an executed
     * compiler, library ABI, debugger or target-platform qualification. */
    static const UmiDeveloperProjectLanguagePack pack = {
        .structure_size = sizeof(UmiDeveloperProjectLanguagePack),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .language_id = "developer.language.kotlin",
        .title = "Kotlin",
        .description = "Kotlin JVM compiler and Java runtime discovery; Kotlin Native is a separate target.",
        .extensions = extensions, .extension_count = sizeof extensions / sizeof extensions[0],
        .build_tools = tools, .build_tool_count = sizeof tools / sizeof tools[0],
        .language_server = "", .formatter = "", .debug_adapter = "",
        .capabilities = UMI_DEVELOPER_PROJECT_LANGUAGE_BUILD | UMI_DEVELOPER_PROJECT_LANGUAGE_RUN,
        .native_language = 0, .compiled_language = 1, .built_in = 1
    };
    return &pack;
}
