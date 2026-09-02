/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_helix_candidates.c
 * PURPOSE: Verify deterministic candidate fitness selection.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/helix/candidate_set.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiHelixCandidateSet set;
    UmiHelixCandidateScore selected;
    umi_helix_candidate_set_init(&set);
    assert(umi_helix_candidate_set_add(&set, "a", 0.82, 1, "ha")
           == UMI_STATUS_OK);
    assert(umi_helix_candidate_set_add(&set, "b", 0.91, 1, "hb")
           == UMI_STATUS_OK);
    assert(umi_helix_candidate_set_add(&set, "c", 0.99, 0, "hc")
           == UMI_STATUS_OK);
    assert(umi_helix_candidate_set_select_best(&set, &selected) == UMI_STATUS_OK);
    assert(strcmp(selected.candidate_id, "b") == 0 && selected.selected);
    return 0;
}
