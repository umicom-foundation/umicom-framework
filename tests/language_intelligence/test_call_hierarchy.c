/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_call_hierarchy.c
 * PURPOSE: Focused regression test for call hierarchy.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/call_hierarchy.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageIntelligenceCallHierarchyEdge edge;
    umi_language_intelligence_call_hierarchy_edge_init(&edge);
    CHECK(umi_language_intelligence_call_hierarchy_edge_set(&edge, "caller", "callee", "calls", 1U) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_call_hierarchy_edge_validate(&edge) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_call_hierarchy_edge_matches_source(&edge, "caller") != 0);
    return 0;
}
