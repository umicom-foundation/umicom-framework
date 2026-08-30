/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_review_finding.c
 *
 * PURPOSE:
 *   Implement the test review finding behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/review_finding.h"
int main(void) {
    UmiAiDevReviewFinding v;
    umi_ai_dev_review_finding_init(&v);
    if (umi_ai_dev_review_finding_configure(&v, "item", "Item", 12U, 3U) != UMI_STATUS_OK) return 1;
    if (umi_ai_dev_review_finding_validate(&v) != UMI_STATUS_OK) return 2;
    if (umi_ai_dev_review_finding_evidence_score(&v, 50U) != 62U) return 3;
    return 0;
}
