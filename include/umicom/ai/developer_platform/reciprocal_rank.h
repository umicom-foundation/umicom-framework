/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/reciprocal_rank.h
 *
 * PURPOSE:
 *   Compute reciprocal-rank fusion scores for multiple retrieval channels.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_RECIPROCAL_RANK_H
#define UMICOM_AI_DEVELOPER_PLATFORM_RECIPROCAL_RANK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev reciprocal rank score data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDevReciprocalRankScore { uint32_t relevance; uint32_t authority; uint32_t freshness; uint32_t penalty; } UmiAiDevReciprocalRankScore;
/**
 * Initialise ai dev reciprocal rank from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_reciprocal_rank_init(UmiAiDevReciprocalRankScore *score);
/**
 * Provide the ai dev reciprocal rank compute operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_dev_reciprocal_rank_compute(const UmiAiDevReciprocalRankScore *score);
/**
 * Provide the ai dev reciprocal rank prefer operation used by this module and its client
 * applications.
 */
int umi_ai_dev_reciprocal_rank_prefer(const UmiAiDevReciprocalRankScore *left, const UmiAiDevReciprocalRankScore *right);
/**
 * Provide the ai dev reciprocal rank weighted operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_dev_reciprocal_rank_weighted(uint32_t primary, uint32_t secondary, uint32_t primary_weight);

#ifdef __cplusplus
}
#endif

#endif
