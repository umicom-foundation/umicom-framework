/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/provider_priority.c
 *
 * PURPOSE:
 *   Implement calculate stable provider priority from explicit preference, project affinity and health.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/provider_priority.h"
#include <string.h>
void umi_language_intelligence_provider_priority_input_init(UmiLanguageIntelligenceProviderPriorityInput *input)
{
    if (input == NULL) return;
    (void)memset(input, 0, sizeof(*input));
    input->struct_size = (uint32_t)sizeof(*input);
    input->api_version = UMI_LANGUAGE_INTELLIGENCE_PROVIDER_PRIORITY_API_VERSION;
    input->enabled = 1;
}
uint64_t umi_language_intelligence_provider_priority_score(const UmiLanguageIntelligenceProviderPriorityInput *input)
{
    uint64_t score;
    uint64_t penalty;
    if (input == NULL || input->enabled == 0 ||
        input->struct_size < sizeof(*input) ||
        input->api_version != UMI_LANGUAGE_INTELLIGENCE_PROVIDER_PRIORITY_API_VERSION)
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
int umi_language_intelligence_provider_priority_prefer(
    const UmiLanguageIntelligenceProviderPriorityInput *left,
    const UmiLanguageIntelligenceProviderPriorityInput *right)
{
    return umi_language_intelligence_provider_priority_score(left) > umi_language_intelligence_provider_priority_score(right);
}
