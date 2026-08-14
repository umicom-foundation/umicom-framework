/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_helix_journal.c
 * PURPOSE: Verify append-only sequence and hash-chain detection.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/helix/journal.h"
int main(void)
{
    UmiHelixJournal journal;
    umi_helix_journal_init(&journal);
    assert(umi_helix_journal_append(
        &journal, UMI_HELIX_JOURNAL_PLAN, "op", "Plan") == UMI_STATUS_OK);
    assert(umi_helix_journal_append(
        &journal, UMI_HELIX_JOURNAL_EVIDENCE, "op", "Build passed")
        == UMI_STATUS_OK);
    assert(umi_helix_journal_verify(&journal));
    journal.entries[0].summary[0] = 'X';
    assert(!umi_helix_journal_verify(&journal));
    return 0;
}
