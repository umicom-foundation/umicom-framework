/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/retrieval_freshness.c
 *
 * PURPOSE:
 *   Adjust retrieval scores using revision freshness.
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
#include "umicom/ai/developer_platform/retrieval_freshness.h"
#include <string.h>
static uint32_t umi_ai_dev_retrieval_freshness_clamp(uint32_t v) { return v > 100U ? 100U : v; }
void umi_ai_dev_retrieval_freshness_init(UmiAiDevRetrievalFreshnessScore *score) { if (score != NULL) memset(score,0,sizeof(*score)); }
uint32_t umi_ai_dev_retrieval_freshness_weighted(uint32_t primary, uint32_t secondary, uint32_t primary_weight) { uint32_t w=primary_weight>100U?100U:primary_weight; uint64_t n=(uint64_t)umi_ai_dev_retrieval_freshness_clamp(primary)*w + (uint64_t)umi_ai_dev_retrieval_freshness_clamp(secondary)*(100U-w); return (uint32_t)(n/100U); }
uint32_t umi_ai_dev_retrieval_freshness_compute(const UmiAiDevRetrievalFreshnessScore *score) {
    uint32_t base;
    if (score == NULL) {
        return 0U;
    }
    base = umi_ai_dev_retrieval_freshness_weighted(score->relevance, score->authority, 65U);
    base = umi_ai_dev_retrieval_freshness_weighted(base, score->freshness, 80U);
    if (score->penalty >= base) {
        return 0U;
    }
    return base - score->penalty;
}
int umi_ai_dev_retrieval_freshness_prefer(const UmiAiDevRetrievalFreshnessScore *left, const UmiAiDevRetrievalFreshnessScore *right) { return umi_ai_dev_retrieval_freshness_compute(left) > umi_ai_dev_retrieval_freshness_compute(right); }
