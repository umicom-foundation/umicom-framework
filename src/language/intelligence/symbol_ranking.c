/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/symbol_ranking.c
 *
 * PURPOSE:
 *   Implement rank symbol-search candidates deterministically.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/symbol_ranking.h"
#include <string.h>
/*
 * Initialise language intelligence symbol ranking input from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_symbol_ranking_input_init(UmiLanguageIntelligenceSymbolRankingInput *input)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL) return;
    (void)memset(input, 0, sizeof(*input));
    input->struct_size = (uint32_t)sizeof(*input);
    input->api_version = UMI_LANGUAGE_INTELLIGENCE_SYMBOL_RANKING_API_VERSION;
    input->enabled = 1;
}
/*
 * Provide the language intelligence symbol ranking score operation used by this module and
 * its client applications.
 */
uint64_t umi_language_intelligence_symbol_ranking_score(const UmiLanguageIntelligenceSymbolRankingInput *input)
{
    uint64_t score;
    uint64_t penalty;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL || input->enabled == 0 ||
        input->struct_size < sizeof(*input) ||
        input->api_version != UMI_LANGUAGE_INTELLIGENCE_SYMBOL_RANKING_API_VERSION)
        return 0U;
    score = (uint64_t)input->base_priority * UINT64_C(1000);
    score = umi_language_intelligence_score_saturating_add(
        score, (uint64_t)input->exact_match * UINT64_C(1000000));
    score = umi_language_intelligence_score_saturating_add(
        score, (uint64_t)input->project_affinity * UINT64_C(100000));
    score = umi_language_intelligence_score_saturating_add(
        score, (uint64_t)input->workspace_affinity * UINT64_C(10000));
    score = umi_language_intelligence_score_saturating_add(
        score, (uint64_t)input->health * UINT64_C(100));
    score = umi_language_intelligence_score_saturating_add(
        score, (uint64_t)input->warm_state * UINT64_C(10));
    penalty = (uint64_t)input->penalty * UINT64_C(1000);
    return penalty >= score ? 0U : score - penalty;
}
/*
 * Provide the language intelligence symbol ranking prefer operation used by this module
 * and its client applications.
 */
int umi_language_intelligence_symbol_ranking_prefer(
    const UmiLanguageIntelligenceSymbolRankingInput *left,
    const UmiLanguageIntelligenceSymbolRankingInput *right)
{
    return umi_language_intelligence_symbol_ranking_score(left) > umi_language_intelligence_symbol_ranking_score(right);
}
