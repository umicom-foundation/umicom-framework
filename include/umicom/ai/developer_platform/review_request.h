/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/review_request.h
 *
 * PURPOSE:
 *   Describe an AI code-review request and its repository baseline.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_REVIEW_REQUEST_H
#define UMICOM_AI_DEVELOPER_PLATFORM_REVIEW_REQUEST_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev review request data shared with callers of this public contract.
 */
typedef struct UmiAiDevReviewRequest {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevReviewRequest;

/**
 * Initialise ai dev review request from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_dev_review_request_init(UmiAiDevReviewRequest *value);
/**
 * Provide the ai dev review request configure operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_review_request_configure(UmiAiDevReviewRequest *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
/**
 * Check that ai dev review request satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ai_dev_review_request_validate(const UmiAiDevReviewRequest *value);
/**
 * Provide the ai dev review request evidence score operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_review_request_evidence_score(const UmiAiDevReviewRequest *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
