/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_inline_candidate.c
 *
 * PURPOSE:
 *   Implement the test inline candidate behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/inline_candidate.h"
int main(void) {
    UmiAiDevInlineCandidate v;
    umi_ai_dev_inline_candidate_init(&v);
    if (umi_ai_dev_inline_candidate_configure(&v, "item", "Item", 12U, 3U) != UMI_STATUS_OK) return 1;
    if (umi_ai_dev_inline_candidate_validate(&v) != UMI_STATUS_OK) return 2;
    if (umi_ai_dev_inline_candidate_evidence_score(&v, 50U) != 62U) return 3;
    return 0;
}
