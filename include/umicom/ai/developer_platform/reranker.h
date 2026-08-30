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

typedef struct UmiAiDevRerankerScore { uint32_t relevance; uint32_t authority; uint32_t freshness; uint32_t penalty; } UmiAiDevRerankerScore;
void umi_ai_dev_reranker_init(UmiAiDevRerankerScore *score);
uint32_t umi_ai_dev_reranker_compute(const UmiAiDevRerankerScore *score);
int umi_ai_dev_reranker_prefer(const UmiAiDevRerankerScore *left, const UmiAiDevRerankerScore *right);
uint32_t umi_ai_dev_reranker_weighted(uint32_t primary, uint32_t secondary, uint32_t primary_weight);

#ifdef __cplusplus
}
#endif

#endif
