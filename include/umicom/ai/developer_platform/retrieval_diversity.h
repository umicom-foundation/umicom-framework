/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/retrieval_diversity.h
 *
 * PURPOSE:
 *   Penalise near-duplicate results to improve context diversity.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_RETRIEVAL_DIVERSITY_H
#define UMICOM_AI_DEVELOPER_PLATFORM_RETRIEVAL_DIVERSITY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev retrieval diversity score data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDevRetrievalDiversityScore { uint32_t relevance; uint32_t authority; uint32_t freshness; uint32_t penalty; } UmiAiDevRetrievalDiversityScore;
/**
 * Initialise ai dev retrieval diversity from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_retrieval_diversity_init(UmiAiDevRetrievalDiversityScore *score);
/**
 * Provide the ai dev retrieval diversity compute operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_retrieval_diversity_compute(const UmiAiDevRetrievalDiversityScore *score);
/**
 * Provide the ai dev retrieval diversity prefer operation used by this module and its
 * client applications.
 */
int umi_ai_dev_retrieval_diversity_prefer(const UmiAiDevRetrievalDiversityScore *left, const UmiAiDevRetrievalDiversityScore *right);
/**
 * Provide the ai dev retrieval diversity weighted operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_retrieval_diversity_weighted(uint32_t primary, uint32_t secondary, uint32_t primary_weight);

#ifdef __cplusplus
}
#endif

#endif
