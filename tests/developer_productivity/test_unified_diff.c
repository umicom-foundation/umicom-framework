/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_unified_diff.c
 *
 * PURPOSE:
 *   Verify Git unified-diff parsing and line statistics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/developer_productivity/unified_diff.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperUnifiedDiff diff;

    assert(umi_developer_unified_diff_parse(
        "--- a/main.c\n"
        "+++ b/main.c\n"
        "@@ -1,2 +1,2 @@\n"
        "-old\n"
        "+new\n"
        " context\n",
        &diff) == UMI_STATUS_OK);
    assert(diff.hunks == 1U);
    assert(diff.additions == 1U);
    assert(diff.deletions == 1U);
    return 0;
}
