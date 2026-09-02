/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/references_index.h
 *
 * PURPOSE:
 *   Maintain bounded reference identities for workspace navigation.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_REFERENCES_INDEX_H
#define UMICOM_LANGUAGE_INTELLIGENCE_REFERENCES_INDEX_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_REFERENCES_INDEX_API_VERSION 1U
/**
 * Represent the language intelligence references index item data shared with callers of
 * this public contract.
 */
typedef struct UmiLanguageIntelligenceReferencesIndexItem {
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char label[UMI_LANGUAGE_INTELLIGENCE_TEXT_CAPACITY];
    uint64_t fingerprint;
    uint32_t priority;
    uint32_t flags;
    int enabled;
} UmiLanguageIntelligenceReferencesIndexItem;
/**
 * Represent the language intelligence references index data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceReferencesIndex {
    uint32_t struct_size;
    uint32_t api_version;
    UmiLanguageIntelligenceReferencesIndexItem items[UMI_LANGUAGE_INTELLIGENCE_ITEM_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiLanguageIntelligenceReferencesIndex;
/**
 * Initialise language intelligence references index from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_references_index_init(UmiLanguageIntelligenceReferencesIndex *catalogue);
/**
 * Provide the language intelligence references index upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_intelligence_references_index_upsert(
    UmiLanguageIntelligenceReferencesIndex *catalogue,
    const UmiLanguageIntelligenceReferencesIndexItem *item);
/**
 * Find language intelligence references index while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiLanguageIntelligenceReferencesIndexItem *umi_language_intelligence_references_index_find(
    const UmiLanguageIntelligenceReferencesIndex *catalogue,
    const char *id);
/**
 * Provide the language intelligence references index best operation used by this module
 * and its client applications.
 */
const UmiLanguageIntelligenceReferencesIndexItem *umi_language_intelligence_references_index_best(
    const UmiLanguageIntelligenceReferencesIndex *catalogue);
#ifdef __cplusplus
}
#endif
#endif
