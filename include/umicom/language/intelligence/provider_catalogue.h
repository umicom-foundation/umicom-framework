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
typedef struct UmiLanguageIntelligenceProviderCatalogueItem {
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char label[UMI_LANGUAGE_INTELLIGENCE_TEXT_CAPACITY];
    uint64_t fingerprint;
    uint32_t priority;
    uint32_t flags;
    int enabled;
} UmiLanguageIntelligenceProviderCatalogueItem;
typedef struct UmiLanguageIntelligenceProviderCatalogue {
    uint32_t struct_size;
    uint32_t api_version;
    UmiLanguageIntelligenceProviderCatalogueItem items[UMI_LANGUAGE_INTELLIGENCE_ITEM_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiLanguageIntelligenceProviderCatalogue;
void umi_language_intelligence_provider_catalogue_init(UmiLanguageIntelligenceProviderCatalogue *catalogue);
UmiStatus umi_language_intelligence_provider_catalogue_upsert(
    UmiLanguageIntelligenceProviderCatalogue *catalogue,
    const UmiLanguageIntelligenceProviderCatalogueItem *item);
const UmiLanguageIntelligenceProviderCatalogueItem *umi_language_intelligence_provider_catalogue_find(
    const UmiLanguageIntelligenceProviderCatalogue *catalogue,
    const char *id);
const UmiLanguageIntelligenceProviderCatalogueItem *umi_language_intelligence_provider_catalogue_best(
    const UmiLanguageIntelligenceProviderCatalogue *catalogue);
#ifdef __cplusplus
}
#endif
#endif
