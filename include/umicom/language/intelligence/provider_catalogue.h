/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/provider_catalogue.h
 *
 * PURPOSE:
 *   Maintain a bounded catalogue of language-intelligence providers.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_CATALOGUE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_CATALOGUE_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_PROVIDER_CATALOGUE_API_VERSION 1U
/**
 * Represent the language intelligence provider catalogue item data shared with callers of
 * this public contract.
 */
typedef struct UmiLanguageIntelligenceProviderCatalogueItem {
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char label[UMI_LANGUAGE_INTELLIGENCE_TEXT_CAPACITY];
    uint64_t fingerprint;
    uint32_t priority;
    uint32_t flags;
    int enabled;
} UmiLanguageIntelligenceProviderCatalogueItem;
/**
 * Represent the language intelligence provider catalogue data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceProviderCatalogue {
    uint32_t struct_size;
    uint32_t api_version;
    UmiLanguageIntelligenceProviderCatalogueItem items[UMI_LANGUAGE_INTELLIGENCE_ITEM_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiLanguageIntelligenceProviderCatalogue;
/**
 * Initialise language intelligence provider catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_provider_catalogue_init(UmiLanguageIntelligenceProviderCatalogue *catalogue);
/**
 * Provide the language intelligence provider catalogue upsert operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_provider_catalogue_upsert(
    UmiLanguageIntelligenceProviderCatalogue *catalogue,
    const UmiLanguageIntelligenceProviderCatalogueItem *item);
/**
 * Find language intelligence provider catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiLanguageIntelligenceProviderCatalogueItem *umi_language_intelligence_provider_catalogue_find(
    const UmiLanguageIntelligenceProviderCatalogue *catalogue,
    const char *id);
/**
 * Provide the language intelligence provider catalogue best operation used by this module
 * and its client applications.
 */
const UmiLanguageIntelligenceProviderCatalogueItem *umi_language_intelligence_provider_catalogue_best(
    const UmiLanguageIntelligenceProviderCatalogue *catalogue);
#ifdef __cplusplus
}
#endif
#endif
