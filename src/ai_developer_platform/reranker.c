/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/reranker.c
 *
 * PURPOSE:
 *   Apply secondary relevance and authority weighting to retrieval candidates.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/reranker.h"
#include <string.h>
static uint32_t umi_ai_dev_reranker_clamp(uint32_t v) { return v > 100U ? 100U : v; }
void umi_ai_dev_reranker_init(UmiAiDevRerankerScore *score) { if (score != NULL) memset(score,0,sizeof(*score)); }
uint32_t umi_ai_dev_reranker_weighted(uint32_t primary, uint32_t secondary, uint32_t primary_weight) { uint32_t w=primary_weight>100U?100U:primary_weight; uint64_t n=(uint64_t)umi_ai_dev_reranker_clamp(primary)*w + (uint64_t)umi_ai_dev_reranker_clamp(secondary)*(100U-w); return (uint32_t)(n/100U); }
uint32_t umi_ai_dev_reranker_compute(const UmiAiDevRerankerScore *score) {
    uint32_t base;
    if (score == NULL) {
        return 0U;
    }
    base = umi_ai_dev_reranker_weighted(score->relevance, score->authority, 65U);
    base = umi_ai_dev_reranker_weighted(base, score->freshness, 80U);
    if (score->penalty >= base) {
        return 0U;
    }
    return base - score->penalty;
}
int umi_ai_dev_reranker_prefer(const UmiAiDevRerankerScore *left, const UmiAiDevRerankerScore *right) { return umi_ai_dev_reranker_compute(left) > umi_ai_dev_reranker_compute(right); }
