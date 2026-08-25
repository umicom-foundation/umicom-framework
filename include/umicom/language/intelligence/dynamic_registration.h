/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/dynamic_registration.h
 *
 * PURPOSE:
 *   Track dynamic provider capability registrations.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_DYNAMIC_REGISTRATION_H
#define UMICOM_LANGUAGE_INTELLIGENCE_DYNAMIC_REGISTRATION_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_DYNAMIC_REGISTRATION_API_VERSION 1U
typedef struct UmiLanguageIntelligenceDynamicRegistrationItem {
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char label[UMI_LANGUAGE_INTELLIGENCE_TEXT_CAPACITY];
    uint64_t fingerprint;
    uint32_t priority;
    uint32_t flags;
    int enabled;
} UmiLanguageIntelligenceDynamicRegistrationItem;
typedef struct UmiLanguageIntelligenceDynamicRegistration {
    uint32_t struct_size;
    uint32_t api_version;
    UmiLanguageIntelligenceDynamicRegistrationItem items[UMI_LANGUAGE_INTELLIGENCE_ITEM_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiLanguageIntelligenceDynamicRegistration;
void umi_language_intelligence_dynamic_registration_init(UmiLanguageIntelligenceDynamicRegistration *catalogue);
UmiStatus umi_language_intelligence_dynamic_registration_upsert(
    UmiLanguageIntelligenceDynamicRegistration *catalogue,
    const UmiLanguageIntelligenceDynamicRegistrationItem *item);
const UmiLanguageIntelligenceDynamicRegistrationItem *umi_language_intelligence_dynamic_registration_find(
    const UmiLanguageIntelligenceDynamicRegistration *catalogue,
    const char *id);
const UmiLanguageIntelligenceDynamicRegistrationItem *umi_language_intelligence_dynamic_registration_best(
    const UmiLanguageIntelligenceDynamicRegistration *catalogue);
#ifdef __cplusplus
}
#endif
#endif
