/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/workspace_configuration.h
 *
 * PURPOSE:
 *   Represent bounded provider-neutral workspace configuration state.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_WORKSPACE_CONFIGURATION_H
#define UMICOM_LANGUAGE_INTELLIGENCE_WORKSPACE_CONFIGURATION_H

#include "umicom/language/intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_CONFIGURATION_API_VERSION 1U

/**
 * Represent the language intelligence workspace configuration data shared with callers of
 * this public contract.
 */
typedef struct UmiLanguageIntelligenceWorkspaceConfiguration {
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
} UmiLanguageIntelligenceWorkspaceConfiguration;

/**
 * Initialise language intelligence workspace configuration from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_workspace_configuration_init(
    UmiLanguageIntelligenceWorkspaceConfiguration *value,
    const char *id);
/**
 * Check that language intelligence workspace configuration satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_language_intelligence_workspace_configuration_validate(
    const UmiLanguageIntelligenceWorkspaceConfiguration *value);
/**
 * Provide the language intelligence workspace configuration set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_language_intelligence_workspace_configuration_set_subject(
    UmiLanguageIntelligenceWorkspaceConfiguration *value,
    const char *subject_id);
/**
 * Provide the language intelligence workspace configuration set detail operation used by
 * this module and its client applications.
 */
UmiStatus umi_language_intelligence_workspace_configuration_set_detail(
    UmiLanguageIntelligenceWorkspaceConfiguration *value,
    const char *detail);
/**
 * Provide the language intelligence workspace configuration touch operation used by this
 * module and its client applications.
 */
void umi_language_intelligence_workspace_configuration_touch(UmiLanguageIntelligenceWorkspaceConfiguration *value);
/**
 * Provide the language intelligence workspace configuration same identity operation used
 * by this module and its client applications.
 */
int umi_language_intelligence_workspace_configuration_same_identity(
    const UmiLanguageIntelligenceWorkspaceConfiguration *left,
    const UmiLanguageIntelligenceWorkspaceConfiguration *right);

#ifdef __cplusplus
}
#endif
#endif
