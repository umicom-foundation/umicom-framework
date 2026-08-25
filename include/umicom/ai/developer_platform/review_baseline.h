/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/review_baseline.h
 *
 * PURPOSE:
 *   Collect acknowledged finding fingerprints for regression-aware review.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_REVIEW_BASELINE_H
#define UMICOM_AI_DEVELOPER_PLATFORM_REVIEW_BASELINE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevReviewBaseline { char ids[UMI_AI_DEV_MEDIUM_CAPACITY][UMI_AI_DEV_ID_CAPACITY]; size_t count; uint64_t revision; } UmiAiDevReviewBaseline;
void umi_ai_dev_review_baseline_init(UmiAiDevReviewBaseline *collection);
UmiStatus umi_ai_dev_review_baseline_add(UmiAiDevReviewBaseline *collection, const char *id);
UmiStatus umi_ai_dev_review_baseline_remove(UmiAiDevReviewBaseline *collection, const char *id);
int umi_ai_dev_review_baseline_contains(const UmiAiDevReviewBaseline *collection, const char *id);
size_t umi_ai_dev_review_baseline_count(const UmiAiDevReviewBaseline *collection);

#ifdef __cplusplus
}
#endif

#endif
