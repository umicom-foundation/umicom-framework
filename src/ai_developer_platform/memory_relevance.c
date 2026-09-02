/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/memory_relevance.c
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
#include "umicom/ai/developer_platform/memory_relevance.h"
#include <string.h>
/*
 * Provide the ai dev memory relevance clamp operation used by this module and its client
 * applications.
 */
static uint32_t umi_ai_dev_memory_relevance_clamp(uint32_t v) { return v > 100U ? 100U : v; }
/*
 * Initialise ai dev memory relevance from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_memory_relevance_init(UmiAiDevMemoryRelevanceScore *score) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (score != NULL) memset(score,0,sizeof(*score)); }
/*
 * Provide the ai dev memory relevance weighted operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_memory_relevance_weighted(uint32_t primary, uint32_t secondary, uint32_t primary_weight) { uint32_t w=primary_weight>100U?100U:primary_weight; uint64_t n=(uint64_t)umi_ai_dev_memory_relevance_clamp(primary)*w + (uint64_t)umi_ai_dev_memory_relevance_clamp(secondary)*(100U-w); return (uint32_t)(n/100U); }
/*
 * Provide the ai dev memory relevance compute operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_dev_memory_relevance_compute(const UmiAiDevMemoryRelevanceScore *score) {
    uint32_t base;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (score == NULL) {
        return 0U;
    }
    base = umi_ai_dev_memory_relevance_weighted(score->relevance, score->authority, 65U);
    base = umi_ai_dev_memory_relevance_weighted(base, score->freshness, 80U);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (score->penalty >= base) {
        return 0U;
    }
    return base - score->penalty;
}
/*
 * Provide the ai dev memory relevance prefer operation used by this module and its client
 * applications.
 */
int umi_ai_dev_memory_relevance_prefer(const UmiAiDevMemoryRelevanceScore *left, const UmiAiDevMemoryRelevanceScore *right) { return umi_ai_dev_memory_relevance_compute(left) > umi_ai_dev_memory_relevance_compute(right); }
