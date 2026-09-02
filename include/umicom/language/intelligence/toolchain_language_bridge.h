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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_TOOLCHAIN_LANGUAGE_BRIDGE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_TOOLCHAIN_LANGUAGE_BRIDGE_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_TOOLCHAIN_LANGUAGE_BRIDGE_API_VERSION 1U

/**
 * Represent the language intelligence toolchain language bridge data shared with callers
 * of this public contract.
 */
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

/**
 * Initialise language intelligence toolchain language bridge from caller-provided values
 * so later operations receive a known state.
 */
void umi_language_intelligence_toolchain_language_bridge_init(UmiLanguageIntelligenceToolchainLanguageBridge *mapping);
/**
 * Copy language intelligence toolchain language bridge into module-owned storage so
 * callers keep ownership of their input values.
 */
UmiStatus umi_language_intelligence_toolchain_language_bridge_set(
    UmiLanguageIntelligenceToolchainLanguageBridge *mapping,
    const char *source_id,
    const char *target_id,
    const char *scope_id);
/**
 * Check that language intelligence toolchain language bridge satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_language_intelligence_toolchain_language_bridge_validate(
    const UmiLanguageIntelligenceToolchainLanguageBridge *mapping);
/**
 * Provide the language intelligence toolchain language bridge matches operation used by
 * this module and its client applications.
 */
int umi_language_intelligence_toolchain_language_bridge_matches(
    const UmiLanguageIntelligenceToolchainLanguageBridge *mapping,
    const char *source_id,
    const char *scope_id);
#ifdef __cplusplus
}
#endif
#endif
