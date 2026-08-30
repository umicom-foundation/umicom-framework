/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_review_request.c
 *
 * PURPOSE:
 *   Implement the test review request behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/review_request.h"
int main(void) {
    UmiAiDevReviewRequest v;
    umi_ai_dev_review_request_init(&v);
    if (umi_ai_dev_review_request_configure(&v, "item", "Item", 12U, 3U) != UMI_STATUS_OK) return 1;
    if (umi_ai_dev_review_request_validate(&v) != UMI_STATUS_OK) return 2;
    if (umi_ai_dev_review_request_evidence_score(&v, 50U) != 62U) return 3;
    return 0;
}
