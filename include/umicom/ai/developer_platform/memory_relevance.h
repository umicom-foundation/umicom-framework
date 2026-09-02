/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/memory_relevance.h
 *
 * PURPOSE:
 *   Rank memory items for the current developer request.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_MEMORY_RELEVANCE_H
#define UMICOM_AI_DEVELOPER_PLATFORM_MEMORY_RELEVANCE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev memory relevance score data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDevMemoryRelevanceScore { uint32_t relevance; uint32_t authority; uint32_t freshness; uint32_t penalty; } UmiAiDevMemoryRelevanceScore;
/**
 * Initialise ai dev memory relevance from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_memory_relevance_init(UmiAiDevMemoryRelevanceScore *score);
/**
 * Provide the ai dev memory relevance compute operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_dev_memory_relevance_compute(const UmiAiDevMemoryRelevanceScore *score);
/**
 * Provide the ai dev memory relevance prefer operation used by this module and its client
 * applications.
 */
int umi_ai_dev_memory_relevance_prefer(const UmiAiDevMemoryRelevanceScore *left, const UmiAiDevMemoryRelevanceScore *right);
/**
 * Provide the ai dev memory relevance weighted operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_memory_relevance_weighted(uint32_t primary, uint32_t secondary, uint32_t primary_weight);

#ifdef __cplusplus
}
#endif

#endif
