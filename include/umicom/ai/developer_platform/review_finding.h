/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/review_finding.h
 *
 * PURPOSE:
 *   Describe one review finding with severity/confidence evidence.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_REVIEW_FINDING_H
#define UMICOM_AI_DEVELOPER_PLATFORM_REVIEW_FINDING_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev review finding data shared with callers of this public contract.
 */
typedef struct UmiAiDevReviewFinding {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevReviewFinding;

/**
 * Initialise ai dev review finding from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_dev_review_finding_init(UmiAiDevReviewFinding *value);
/**
 * Provide the ai dev review finding configure operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_review_finding_configure(UmiAiDevReviewFinding *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
/**
 * Check that ai dev review finding satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ai_dev_review_finding_validate(const UmiAiDevReviewFinding *value);
/**
 * Provide the ai dev review finding evidence score operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_review_finding_evidence_score(const UmiAiDevReviewFinding *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
