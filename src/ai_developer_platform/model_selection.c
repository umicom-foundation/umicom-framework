/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/model_selection.c
 *
 * PURPOSE:
 *   Rank eligible models deterministically for a requested developer operation.
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
#include "umicom/ai/developer_platform/model_selection.h"
#include <string.h>
/*
 * Provide the ai dev model selection clamp operation used by this module and its client
 * applications.
 */
static uint32_t umi_ai_dev_model_selection_clamp(uint32_t v) { return v > 100U ? 100U : v; }
/*
 * Initialise ai dev model selection from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_model_selection_init(UmiAiDevModelSelectionScore *score) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (score != NULL) memset(score,0,sizeof(*score)); }
/*
 * Provide the ai dev model selection weighted operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_dev_model_selection_weighted(uint32_t primary, uint32_t secondary, uint32_t primary_weight) { uint32_t w=primary_weight>100U?100U:primary_weight; uint64_t n=(uint64_t)umi_ai_dev_model_selection_clamp(primary)*w + (uint64_t)umi_ai_dev_model_selection_clamp(secondary)*(100U-w); return (uint32_t)(n/100U); }
/*
 * Provide the ai dev model selection compute operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_dev_model_selection_compute(const UmiAiDevModelSelectionScore *score) {
    uint32_t base;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (score == NULL) {
        return 0U;
    }
    base = umi_ai_dev_model_selection_weighted(score->relevance, score->authority, 65U);
    base = umi_ai_dev_model_selection_weighted(base, score->freshness, 80U);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (score->penalty >= base) {
        return 0U;
    }
    return base - score->penalty;
}
/*
 * Provide the ai dev model selection prefer operation used by this module and its client
 * applications.
 */
int umi_ai_dev_model_selection_prefer(const UmiAiDevModelSelectionScore *left, const UmiAiDevModelSelectionScore *right) { return umi_ai_dev_model_selection_compute(left) > umi_ai_dev_model_selection_compute(right); }
