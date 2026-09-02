/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/semantic_legend.h
 *
 * PURPOSE:
 *   Maintain token-type/modifier legend identities.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_SEMANTIC_LEGEND_H
#define UMICOM_LANGUAGE_INTELLIGENCE_SEMANTIC_LEGEND_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_LEGEND_API_VERSION 1U
/**
 * Represent the language intelligence semantic legend item data shared with callers of
 * this public contract.
 */
typedef struct UmiLanguageIntelligenceSemanticLegendItem {
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char label[UMI_LANGUAGE_INTELLIGENCE_TEXT_CAPACITY];
    uint64_t fingerprint;
    uint32_t priority;
    uint32_t flags;
    int enabled;
} UmiLanguageIntelligenceSemanticLegendItem;
/**
 * Represent the language intelligence semantic legend data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceSemanticLegend {
    uint32_t struct_size;
    uint32_t api_version;
    UmiLanguageIntelligenceSemanticLegendItem items[UMI_LANGUAGE_INTELLIGENCE_ITEM_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiLanguageIntelligenceSemanticLegend;
/**
 * Initialise language intelligence semantic legend from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_semantic_legend_init(UmiLanguageIntelligenceSemanticLegend *catalogue);
/**
 * Provide the language intelligence semantic legend upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_intelligence_semantic_legend_upsert(
    UmiLanguageIntelligenceSemanticLegend *catalogue,
    const UmiLanguageIntelligenceSemanticLegendItem *item);
/**
 * Find language intelligence semantic legend while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiLanguageIntelligenceSemanticLegendItem *umi_language_intelligence_semantic_legend_find(
    const UmiLanguageIntelligenceSemanticLegend *catalogue,
    const char *id);
/**
 * Provide the language intelligence semantic legend best operation used by this module and
 * its client applications.
 */
const UmiLanguageIntelligenceSemanticLegendItem *umi_language_intelligence_semantic_legend_best(
    const UmiLanguageIntelligenceSemanticLegend *catalogue);
#ifdef __cplusplus
}
#endif
#endif
