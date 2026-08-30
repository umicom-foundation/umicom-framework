/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/hybrid_retrieval.h
 *
 * PURPOSE:
 *   Combine lexical and semantic scores under explicit weighting.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_HYBRID_RETRIEVAL_H
#define UMICOM_AI_DEVELOPER_PLATFORM_HYBRID_RETRIEVAL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevHybridRetrievalScore { uint32_t relevance; uint32_t authority; uint32_t freshness; uint32_t penalty; } UmiAiDevHybridRetrievalScore;
void umi_ai_dev_hybrid_retrieval_init(UmiAiDevHybridRetrievalScore *score);
uint32_t umi_ai_dev_hybrid_retrieval_compute(const UmiAiDevHybridRetrievalScore *score);
int umi_ai_dev_hybrid_retrieval_prefer(const UmiAiDevHybridRetrievalScore *left, const UmiAiDevHybridRetrievalScore *right);
uint32_t umi_ai_dev_hybrid_retrieval_weighted(uint32_t primary, uint32_t secondary, uint32_t primary_weight);

#ifdef __cplusplus
}
#endif

#endif
