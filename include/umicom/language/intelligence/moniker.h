/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/moniker.h
 *
 * PURPOSE:
 *   Represent stable symbol monikers for cross-project identity.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_MONIKER_H
#define UMICOM_LANGUAGE_INTELLIGENCE_MONIKER_H

#include "umicom/language/intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INTELLIGENCE_MONIKER_API_VERSION 1U

typedef struct UmiLanguageIntelligenceMoniker {
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
} UmiLanguageIntelligenceMoniker;

void umi_language_intelligence_moniker_init(
    UmiLanguageIntelligenceMoniker *value,
    const char *id);
UmiStatus umi_language_intelligence_moniker_validate(
    const UmiLanguageIntelligenceMoniker *value);
UmiStatus umi_language_intelligence_moniker_set_subject(
    UmiLanguageIntelligenceMoniker *value,
    const char *subject_id);
UmiStatus umi_language_intelligence_moniker_set_detail(
    UmiLanguageIntelligenceMoniker *value,
    const char *detail);
void umi_language_intelligence_moniker_touch(UmiLanguageIntelligenceMoniker *value);
int umi_language_intelligence_moniker_same_identity(
    const UmiLanguageIntelligenceMoniker *left,
    const UmiLanguageIntelligenceMoniker *right);

#ifdef __cplusplus
}
#endif
#endif
