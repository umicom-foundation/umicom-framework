/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_duplicate_evidence.c
 *
 * PURPOSE:
 *   Implement the test codeguard duplicate evidence behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Duplicate evidence v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "umicom/codeguard/duplicate_evidence.h"
int main(void)
{
    UmiCodeGuardDuplicateFile first = {"src/a.c",17U,27U,120U};
    UmiCodeGuardDuplicateFile second = {"src/b.c",17U,27U,120U};
    UmiCodeGuardDuplicateCandidate candidate;
    UmiCodeGuardDuplicateReview review = {0};
    assert(umi_codeguard_duplicate_candidate_build(&first,&second,&candidate) == UMI_STATUS_OK);
    assert(candidate.byte_identical && candidate.similarity_percent == 100U && !candidate.automatic_action_permitted);
    assert(umi_codeguard_duplicate_review_add(&review,&candidate) == UMI_STATUS_OK);
    assert(umi_codeguard_duplicate_review_disposition(&review,0U,UMI_CODEGUARD_CANDIDATE_KEEP_SEPARATE) == UMI_STATUS_OK);
    assert(!review.items[0].automatic_action_permitted);
    return 0;
}
