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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiAiDevReviewRequest {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevReviewRequest;

void umi_ai_dev_review_request_init(UmiAiDevReviewRequest *value);
UmiStatus umi_ai_dev_review_request_configure(UmiAiDevReviewRequest *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
UmiStatus umi_ai_dev_review_request_validate(const UmiAiDevReviewRequest *value);
uint32_t umi_ai_dev_review_request_evidence_score(const UmiAiDevReviewRequest *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
