/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/retrieval_freshness.h
 *
 * PURPOSE:
 *   Adjust retrieval scores using revision freshness.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_RETRIEVAL_FRESHNESS_H
#define UMICOM_AI_DEVELOPER_PLATFORM_RETRIEVAL_FRESHNESS_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev retrieval freshness score data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDevRetrievalFreshnessScore { uint32_t relevance; uint32_t authority; uint32_t freshness; uint32_t penalty; } UmiAiDevRetrievalFreshnessScore;
/**
 * Initialise ai dev retrieval freshness from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_retrieval_freshness_init(UmiAiDevRetrievalFreshnessScore *score);
/**
 * Provide the ai dev retrieval freshness compute operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_retrieval_freshness_compute(const UmiAiDevRetrievalFreshnessScore *score);
/**
 * Provide the ai dev retrieval freshness prefer operation used by this module and its
 * client applications.
 */
int umi_ai_dev_retrieval_freshness_prefer(const UmiAiDevRetrievalFreshnessScore *left, const UmiAiDevRetrievalFreshnessScore *right);
/**
 * Provide the ai dev retrieval freshness weighted operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_retrieval_freshness_weighted(uint32_t primary, uint32_t secondary, uint32_t primary_weight);

#ifdef __cplusplus
}
#endif

#endif
