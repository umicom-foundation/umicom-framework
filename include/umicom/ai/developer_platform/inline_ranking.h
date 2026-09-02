/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/inline_ranking.h
 *
 * PURPOSE:
 *   Rank inline candidates using confidence and edit-cost signals.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_INLINE_RANKING_H
#define UMICOM_AI_DEVELOPER_PLATFORM_INLINE_RANKING_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev inline ranking score data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDevInlineRankingScore { uint32_t relevance; uint32_t authority; uint32_t freshness; uint32_t penalty; } UmiAiDevInlineRankingScore;
/**
 * Initialise ai dev inline ranking from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_dev_inline_ranking_init(UmiAiDevInlineRankingScore *score);
/**
 * Provide the ai dev inline ranking compute operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_dev_inline_ranking_compute(const UmiAiDevInlineRankingScore *score);
/**
 * Provide the ai dev inline ranking prefer operation used by this module and its client
 * applications.
 */
int umi_ai_dev_inline_ranking_prefer(const UmiAiDevInlineRankingScore *left, const UmiAiDevInlineRankingScore *right);
/**
 * Provide the ai dev inline ranking weighted operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_dev_inline_ranking_weighted(uint32_t primary, uint32_t secondary, uint32_t primary_weight);

#ifdef __cplusplus
}
#endif

#endif
