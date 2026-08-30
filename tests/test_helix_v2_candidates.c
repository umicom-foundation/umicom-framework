/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_helix_v2_candidates.c
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
#include "umicom/helix/candidate_set_v2.h"
int main(void)
{
    UmiHelixCandidateSetV2 set;
    UmiHelixCandidateScoreV2 selected;
    umi_helix_candidate_set_v2_init(&set);
    assert(umi_helix_candidate_set_v2_add(&set, "a", 0.82, 1, "ha")
           == UMI_STATUS_OK);
    assert(umi_helix_candidate_set_v2_add(&set, "b", 0.91, 1, "hb")
           == UMI_STATUS_OK);
    assert(umi_helix_candidate_set_v2_add(&set, "c", 0.99, 0, "hc")
           == UMI_STATUS_OK);
    assert(umi_helix_candidate_set_v2_select_best(&set, &selected) == UMI_STATUS_OK);
    assert(strcmp(selected.candidate_id, "b") == 0 && selected.selected);
    return 0;
}
