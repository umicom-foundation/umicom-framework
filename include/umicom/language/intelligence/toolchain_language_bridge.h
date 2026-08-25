/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/toolchain_language_bridge.h
 *
 * PURPOSE:
 *   Map language profiles to Framework developer-kit/toolchain identities.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_TOOLCHAIN_LANGUAGE_BRIDGE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_TOOLCHAIN_LANGUAGE_BRIDGE_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_TOOLCHAIN_LANGUAGE_BRIDGE_API_VERSION 1U

typedef struct UmiLanguageIntelligenceToolchainLanguageBridge {
    uint32_t struct_size;
    uint32_t api_version;
    char source_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char target_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char scope_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    uint32_t priority;
    int enabled;
    uint64_t revision;
} UmiLanguageIntelligenceToolchainLanguageBridge;

void umi_language_intelligence_toolchain_language_bridge_init(UmiLanguageIntelligenceToolchainLanguageBridge *mapping);
UmiStatus umi_language_intelligence_toolchain_language_bridge_set(
    UmiLanguageIntelligenceToolchainLanguageBridge *mapping,
    const char *source_id,
    const char *target_id,
    const char *scope_id);
UmiStatus umi_language_intelligence_toolchain_language_bridge_validate(
    const UmiLanguageIntelligenceToolchainLanguageBridge *mapping);
int umi_language_intelligence_toolchain_language_bridge_matches(
    const UmiLanguageIntelligenceToolchainLanguageBridge *mapping,
    const char *source_id,
    const char *scope_id);
#ifdef __cplusplus
}
#endif
#endif
