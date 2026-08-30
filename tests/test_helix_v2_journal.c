/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_helix_v2_journal.c
 * PURPOSE: Verify append-only sequence and hash-chain detection.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/helix/journal_v2.h"
int main(void)
{
    UmiHelixJournalV2 journal;
    umi_helix_journal_v2_init(&journal);
    assert(umi_helix_journal_v2_append(
        &journal, UMI_HELIX_JOURNAL_PLAN, "op", "Plan") == UMI_STATUS_OK);
    assert(umi_helix_journal_v2_append(
        &journal, UMI_HELIX_JOURNAL_EVIDENCE, "op", "Build passed")
        == UMI_STATUS_OK);
    assert(umi_helix_journal_v2_verify(&journal));
    journal.entries[0].summary[0] = 'X';
    assert(!umi_helix_journal_v2_verify(&journal));
    return 0;
}
