/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/provider_selection.c
 *
 * PURPOSE:
 *   Implement rank provider candidates deterministically for a requested document and capability set.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/provider_selection.h"
#include <string.h>
/*
 * Initialise language intelligence provider selection input from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_provider_selection_input_init(UmiLanguageIntelligenceProviderSelectionInput *input)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL) return;
    (void)memset(input, 0, sizeof(*input));
    input->struct_size = (uint32_t)sizeof(*input);
    input->api_version = UMI_LANGUAGE_INTELLIGENCE_PROVIDER_SELECTION_API_VERSION;
    input->enabled = 1;
}
/*
 * Provide the language intelligence provider selection score operation used by this module
 * and its client applications.
 */
uint64_t umi_language_intelligence_provider_selection_score(const UmiLanguageIntelligenceProviderSelectionInput *input)
{
    uint64_t score;
    uint64_t penalty;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL || input->enabled == 0 ||
        input->struct_size < sizeof(*input) ||
        input->api_version != UMI_LANGUAGE_INTELLIGENCE_PROVIDER_SELECTION_API_VERSION)
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
 * Provide the language intelligence provider selection prefer operation used by this
 * module and its client applications.
 */
int umi_language_intelligence_provider_selection_prefer(
    const UmiLanguageIntelligenceProviderSelectionInput *left,
    const UmiLanguageIntelligenceProviderSelectionInput *right)
{
    return umi_language_intelligence_provider_selection_score(left) > umi_language_intelligence_provider_selection_score(right);
}
