/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/provider_descriptor.h
 *
 * PURPOSE:
 *   Describe one language-intelligence provider and its immutable identity/capability metadata.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_DESCRIPTOR_H
#define UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_DESCRIPTOR_H

#include "umicom/language/intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INTELLIGENCE_PROVIDER_DESCRIPTOR_API_VERSION 1U

typedef struct UmiLanguageIntelligenceProviderDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char subject_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char detail[UMI_LANGUAGE_INTELLIGENCE_TEXT_CAPACITY];
    UmiLanguageIntelligenceRange range;
    uint64_t revision;
    uint32_t priority;
    uint32_t flags;
    int enabled;
} UmiLanguageIntelligenceProviderDescriptor;

void umi_language_intelligence_provider_descriptor_init(
    UmiLanguageIntelligenceProviderDescriptor *value,
    const char *id);
UmiStatus umi_language_intelligence_provider_descriptor_validate(
    const UmiLanguageIntelligenceProviderDescriptor *value);
UmiStatus umi_language_intelligence_provider_descriptor_set_subject(
    UmiLanguageIntelligenceProviderDescriptor *value,
    const char *subject_id);
UmiStatus umi_language_intelligence_provider_descriptor_set_detail(
    UmiLanguageIntelligenceProviderDescriptor *value,
    const char *detail);
void umi_language_intelligence_provider_descriptor_touch(UmiLanguageIntelligenceProviderDescriptor *value);
int umi_language_intelligence_provider_descriptor_same_identity(
    const UmiLanguageIntelligenceProviderDescriptor *left,
    const UmiLanguageIntelligenceProviderDescriptor *right);

#ifdef __cplusplus
}
#endif
#endif
