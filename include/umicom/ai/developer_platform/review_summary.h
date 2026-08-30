/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/review_summary.h
 *
 * PURPOSE:
 *   Aggregate review counts and determine review readiness.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_REVIEW_SUMMARY_H
#define UMICOM_AI_DEVELOPER_PLATFORM_REVIEW_SUMMARY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevReviewSummary {
    UmiAiDevState state;
    uint32_t successes;
    uint32_t failures;
    uint32_t pending;
    uint64_t revision;
} UmiAiDevReviewSummary;

void umi_ai_dev_review_summary_init(UmiAiDevReviewSummary *value);
UmiStatus umi_ai_dev_review_summary_record(UmiAiDevReviewSummary *value, int success);
void umi_ai_dev_review_summary_set_pending(UmiAiDevReviewSummary *value, uint32_t pending);
uint32_t umi_ai_dev_review_summary_health_score(const UmiAiDevReviewSummary *value);
int umi_ai_dev_review_summary_ready(const UmiAiDevReviewSummary *value);

#ifdef __cplusplus
}
#endif

#endif
