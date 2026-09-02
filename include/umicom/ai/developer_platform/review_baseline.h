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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the ai dev review baseline data shared with callers of this public contract.
 */
typedef struct UmiAiDevReviewBaseline { char ids[UMI_AI_DEV_MEDIUM_CAPACITY][UMI_AI_DEV_ID_CAPACITY]; size_t count; uint64_t revision; } UmiAiDevReviewBaseline;
/**
 * Initialise ai dev review baseline from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_review_baseline_init(UmiAiDevReviewBaseline *collection);
/**
 * Add ai dev review baseline only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ai_dev_review_baseline_add(UmiAiDevReviewBaseline *collection, const char *id);
/**
 * Remove ai dev review baseline while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ai_dev_review_baseline_remove(UmiAiDevReviewBaseline *collection, const char *id);
/**
 * Provide the ai dev review baseline contains operation used by this module and its client
 * applications.
 */
int umi_ai_dev_review_baseline_contains(const UmiAiDevReviewBaseline *collection, const char *id);
/**
 * Return the number of records represented by ai dev review baseline without changing
 * their state.
 */
size_t umi_ai_dev_review_baseline_count(const UmiAiDevReviewBaseline *collection);

#ifdef __cplusplus
}
#endif

#endif
