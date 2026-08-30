/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_vcs_diff.c
 *
 * PURPOSE:
 *   Verify Git numstat parsing and aggregate totals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/vcs/diff.h"

int main(void)
{
    UmiVcsDiffStat stats[4];
    size_t count = 0U;
    size_t insertions = 0U;
    size_t deletions = 0U;
    size_t binary = 0U;
    assert(umi_vcs_diff_parse_numstat("10\t2\tsrc/main.c\n"
                                         "-\t-\tasset.png\n",
                                         stats,
                                         4U,
                                         &count) == UMI_STATUS_OK);
    assert(count == 2U);
    umi_vcs_diff_totals(stats, count, &insertions, &deletions, &binary);
    assert(insertions == 10U);
    assert(deletions == 2U);
    assert(binary == 1U);
    return 0;
}
