/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/symbol_ranking.h
 *
 * PURPOSE:
 *   Rank symbol-search candidates deterministically.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_SYMBOL_RANKING_H
#define UMICOM_LANGUAGE_INTELLIGENCE_SYMBOL_RANKING_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_SYMBOL_RANKING_API_VERSION 1U
/**
 * Represent the language intelligence symbol ranking input data shared with callers of
 * this public contract.
 */
typedef struct UmiLanguageIntelligenceSymbolRankingInput {
    uint32_t struct_size;
    uint32_t api_version;
    uint32_t base_priority;
    uint32_t exact_match;
    uint32_t project_affinity;
    uint32_t workspace_affinity;
    uint32_t health;
    uint32_t warm_state;
    uint32_t penalty;
    int enabled;
} UmiLanguageIntelligenceSymbolRankingInput;
/**
 * Initialise language intelligence symbol ranking input from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_symbol_ranking_input_init(UmiLanguageIntelligenceSymbolRankingInput *input);
/**
 * Provide the language intelligence symbol ranking score operation used by this module and
 * its client applications.
 */
uint64_t umi_language_intelligence_symbol_ranking_score(const UmiLanguageIntelligenceSymbolRankingInput *input);
/**
 * Provide the language intelligence symbol ranking prefer operation used by this module
 * and its client applications.
 */
int umi_language_intelligence_symbol_ranking_prefer(
    const UmiLanguageIntelligenceSymbolRankingInput *left,
    const UmiLanguageIntelligenceSymbolRankingInput *right);
#ifdef __cplusplus
}
#endif
#endif
