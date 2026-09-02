/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/reranker.h
 *
 * PURPOSE:
 *   Apply secondary relevance and authority weighting to retrieval candidates.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_RERANKER_H
#define UMICOM_AI_DEVELOPER_PLATFORM_RERANKER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev reranker score data shared with callers of this public contract.
 */
typedef struct UmiAiDevRerankerScore { uint32_t relevance; uint32_t authority; uint32_t freshness; uint32_t penalty; } UmiAiDevRerankerScore;
/**
 * Initialise ai dev reranker from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_dev_reranker_init(UmiAiDevRerankerScore *score);
/**
 * Provide the ai dev reranker compute operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_dev_reranker_compute(const UmiAiDevRerankerScore *score);
/**
 * Provide the ai dev reranker prefer operation used by this module and its client
 * applications.
 */
int umi_ai_dev_reranker_prefer(const UmiAiDevRerankerScore *left, const UmiAiDevRerankerScore *right);
/**
 * Provide the ai dev reranker weighted operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_dev_reranker_weighted(uint32_t primary, uint32_t secondary, uint32_t primary_weight);

#ifdef __cplusplus
}
#endif

#endif
