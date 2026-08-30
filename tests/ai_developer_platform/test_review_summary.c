/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_review_summary.c
 *
 * PURPOSE:
 *   Implement the test review summary behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/review_summary.h"
int main(void) { UmiAiDevReviewSummary v; umi_ai_dev_review_summary_init(&v); if (umi_ai_dev_review_summary_record(&v, 1) != UMI_STATUS_OK) return 1; if (!umi_ai_dev_review_summary_ready(&v)) return 2; if (umi_ai_dev_review_summary_health_score(&v) != 100U) return 3; return 0; }
