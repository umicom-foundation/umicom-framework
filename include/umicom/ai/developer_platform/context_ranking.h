/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/context_ranking.h
 *
 * PURPOSE:
 *   Rank context candidates by relevance, recency and authority.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_RANKING_H
#define UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_RANKING_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev context ranking score data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDevContextRankingScore { uint32_t relevance; uint32_t authority; uint32_t freshness; uint32_t penalty; } UmiAiDevContextRankingScore;
/**
 * Initialise ai dev context ranking from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_context_ranking_init(UmiAiDevContextRankingScore *score);
/**
 * Provide the ai dev context ranking compute operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_dev_context_ranking_compute(const UmiAiDevContextRankingScore *score);
/**
 * Provide the ai dev context ranking prefer operation used by this module and its client
 * applications.
 */
int umi_ai_dev_context_ranking_prefer(const UmiAiDevContextRankingScore *left, const UmiAiDevContextRankingScore *right);
/**
 * Provide the ai dev context ranking weighted operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_dev_context_ranking_weighted(uint32_t primary, uint32_t secondary, uint32_t primary_weight);

#ifdef __cplusplus
}
#endif

#endif
