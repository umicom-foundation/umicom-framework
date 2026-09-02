/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_compilation_database_bridge.c
 * PURPOSE: Focused regression test for compilation database bridge.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/compilation_database_bridge.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageIntelligenceCompilationDatabaseBridge mapping;
    umi_language_intelligence_compilation_database_bridge_init(&mapping);
    CHECK(umi_language_intelligence_compilation_database_bridge_set(&mapping, "c", "clangd", "workspace") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_compilation_database_bridge_validate(&mapping) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_compilation_database_bridge_matches(&mapping, "c", "workspace") != 0);
    CHECK(umi_language_intelligence_compilation_database_bridge_matches(&mapping, "cpp", "workspace") == 0);
    return 0;
}
