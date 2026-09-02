/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/search_types.c
 *
 * PURPOSE:
 *   Implement provider-neutral search category text and deterministic scoring.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/search_types.h"

#include <ctype.h>
#include <string.h>

/*
 * Provide the ascii equal folded operation used by this module and its client
 * applications.
 */
static int ascii_equal_folded(char left, char right)
{
    return tolower((unsigned char)left) ==
           tolower((unsigned char)right);
}

/* Provide the find folded operation used by this module and its client applications. */
static const char *find_folded(const char *text, const char *query)
{
    const char *start;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || query == NULL || query[0] == '\0') return text;

    /* Visit each bounded item once so every record receives the same rule. */
    for (start = text; *start != '\0'; ++start) {
        const char *a = start;
        const char *b = query;

        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*a != '\0' && *b != '\0' &&
               ascii_equal_folded(*a, *b)) {
            ++a;
            ++b;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (*b == '\0') return start;
    }

    return NULL;
}

/*
 * Provide the developer workbench search kind text operation used by this module and its
 * client applications.
 */
const char *umi_developer_workbench_search_kind_text(
    UmiDeveloperWorkbenchSearchKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_DEVELOPER_WORKBENCH_SEARCH_COMMAND: return "command";
        case UMI_DEVELOPER_WORKBENCH_SEARCH_FILE: return "file";
        case UMI_DEVELOPER_WORKBENCH_SEARCH_RECENT_PROJECT:
            return "recent-project";
        case UMI_DEVELOPER_WORKBENCH_SEARCH_SYMBOL: return "symbol";
        case UMI_DEVELOPER_WORKBENCH_SEARCH_SETTING: return "setting";
        case UMI_DEVELOPER_WORKBENCH_SEARCH_SOURCE_CONTROL:
            return "source-control";
        default: return "unknown";
    }
}

/*
 * Provide the developer workbench search score operation used by this module and its
 * client applications.
 */
int32_t umi_developer_workbench_search_score(
    const char *query,
    const char *primary,
    const char *secondary)
{
    const char *primary_match;
    const char *secondary_match;
    size_t offset;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || query[0] == '\0') return 100;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (primary == NULL) primary = "";
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (secondary == NULL) secondary = "";

    primary_match = find_folded(primary, query);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (primary_match != NULL) {
        offset = (size_t)(primary_match - primary);
        return offset == 0U
            ? 1000
            : 800 - (int32_t)(offset < 200U ? offset : 200U);
    }

    secondary_match = find_folded(secondary, query);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (secondary_match != NULL) {
        offset = (size_t)(secondary_match - secondary);
        return 500 - (int32_t)(offset < 200U ? offset : 200U);
    }

    return -1;
}
