/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/semantic_query.h
 *
 * PURPOSE:
 *   Score semantic retrieval candidates with bounded integer similarity values.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_SEMANTIC_QUERY_H
#define UMICOM_AI_DEVELOPER_PLATFORM_SEMANTIC_QUERY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevSemanticQueryScore { uint32_t relevance; uint32_t authority; uint32_t freshness; uint32_t penalty; } UmiAiDevSemanticQueryScore;
void umi_ai_dev_semantic_query_init(UmiAiDevSemanticQueryScore *score);
uint32_t umi_ai_dev_semantic_query_compute(const UmiAiDevSemanticQueryScore *score);
int umi_ai_dev_semantic_query_prefer(const UmiAiDevSemanticQueryScore *left, const UmiAiDevSemanticQueryScore *right);
uint32_t umi_ai_dev_semantic_query_weighted(uint32_t primary, uint32_t secondary, uint32_t primary_weight);

#ifdef __cplusplus
}
#endif

#endif
