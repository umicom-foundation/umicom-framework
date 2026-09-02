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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_DYNAMIC_REGISTRATION_H
#define UMICOM_LANGUAGE_INTELLIGENCE_DYNAMIC_REGISTRATION_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_DYNAMIC_REGISTRATION_API_VERSION 1U
/**
 * Represent the language intelligence dynamic registration item data shared with callers
 * of this public contract.
 */
typedef struct UmiLanguageIntelligenceDynamicRegistrationItem {
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char label[UMI_LANGUAGE_INTELLIGENCE_TEXT_CAPACITY];
    uint64_t fingerprint;
    uint32_t priority;
    uint32_t flags;
    int enabled;
} UmiLanguageIntelligenceDynamicRegistrationItem;
/**
 * Represent the language intelligence dynamic registration data shared with callers of
 * this public contract.
 */
typedef struct UmiLanguageIntelligenceDynamicRegistration {
    uint32_t struct_size;
    uint32_t api_version;
    UmiLanguageIntelligenceDynamicRegistrationItem items[UMI_LANGUAGE_INTELLIGENCE_ITEM_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiLanguageIntelligenceDynamicRegistration;
/**
 * Initialise language intelligence dynamic registration from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_dynamic_registration_init(UmiLanguageIntelligenceDynamicRegistration *catalogue);
/**
 * Provide the language intelligence dynamic registration upsert operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_dynamic_registration_upsert(
    UmiLanguageIntelligenceDynamicRegistration *catalogue,
    const UmiLanguageIntelligenceDynamicRegistrationItem *item);
/**
 * Find language intelligence dynamic registration while leaving the underlying catalogue
 * or model owned by this module.
 */
const UmiLanguageIntelligenceDynamicRegistrationItem *umi_language_intelligence_dynamic_registration_find(
    const UmiLanguageIntelligenceDynamicRegistration *catalogue,
    const char *id);
/**
 * Provide the language intelligence dynamic registration best operation used by this
 * module and its client applications.
 */
const UmiLanguageIntelligenceDynamicRegistrationItem *umi_language_intelligence_dynamic_registration_best(
    const UmiLanguageIntelligenceDynamicRegistration *catalogue);
#ifdef __cplusplus
}
#endif
#endif
