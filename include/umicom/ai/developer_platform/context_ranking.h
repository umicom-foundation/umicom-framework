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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiAiDevContextRankingScore { uint32_t relevance; uint32_t authority; uint32_t freshness; uint32_t penalty; } UmiAiDevContextRankingScore;
void umi_ai_dev_context_ranking_init(UmiAiDevContextRankingScore *score);
uint32_t umi_ai_dev_context_ranking_compute(const UmiAiDevContextRankingScore *score);
int umi_ai_dev_context_ranking_prefer(const UmiAiDevContextRankingScore *left, const UmiAiDevContextRankingScore *right);
uint32_t umi_ai_dev_context_ranking_weighted(uint32_t primary, uint32_t secondary, uint32_t primary_weight);

#ifdef __cplusplus
}
#endif

#endif
