/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/symbol_index.h
 *
 * PURPOSE:
 *   Maintain a bounded workspace symbol index.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_SYMBOL_INDEX_H
#define UMICOM_LANGUAGE_INTELLIGENCE_SYMBOL_INDEX_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_SYMBOL_INDEX_API_VERSION 1U
typedef struct UmiLanguageIntelligenceSymbolIndexItem {
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char label[UMI_LANGUAGE_INTELLIGENCE_TEXT_CAPACITY];
    uint64_t fingerprint;
    uint32_t priority;
    uint32_t flags;
    int enabled;
} UmiLanguageIntelligenceSymbolIndexItem;
typedef struct UmiLanguageIntelligenceSymbolIndex {
    uint32_t struct_size;
    uint32_t api_version;
    UmiLanguageIntelligenceSymbolIndexItem items[UMI_LANGUAGE_INTELLIGENCE_ITEM_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiLanguageIntelligenceSymbolIndex;
void umi_language_intelligence_symbol_index_init(UmiLanguageIntelligenceSymbolIndex *catalogue);
UmiStatus umi_language_intelligence_symbol_index_upsert(
    UmiLanguageIntelligenceSymbolIndex *catalogue,
    const UmiLanguageIntelligenceSymbolIndexItem *item);
const UmiLanguageIntelligenceSymbolIndexItem *umi_language_intelligence_symbol_index_find(
    const UmiLanguageIntelligenceSymbolIndex *catalogue,
    const char *id);
const UmiLanguageIntelligenceSymbolIndexItem *umi_language_intelligence_symbol_index_best(
    const UmiLanguageIntelligenceSymbolIndex *catalogue);
#ifdef __cplusplus
}
#endif
#endif
