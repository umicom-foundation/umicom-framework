/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/workspace_folder.h
 *
 * PURPOSE:
 *   Describe one language-intelligence workspace folder with stable identity.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_WORKSPACE_FOLDER_H
#define UMICOM_LANGUAGE_INTELLIGENCE_WORKSPACE_FOLDER_H

#include "umicom/language/intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_FOLDER_API_VERSION 1U

/**
 * Represent the language intelligence workspace folder data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceWorkspaceFolder {
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
} UmiLanguageIntelligenceWorkspaceFolder;

/**
 * Initialise language intelligence workspace folder from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_workspace_folder_init(
    UmiLanguageIntelligenceWorkspaceFolder *value,
    const char *id);
/**
 * Check that language intelligence workspace folder satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_language_intelligence_workspace_folder_validate(
    const UmiLanguageIntelligenceWorkspaceFolder *value);
/**
 * Provide the language intelligence workspace folder set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_workspace_folder_set_subject(
    UmiLanguageIntelligenceWorkspaceFolder *value,
    const char *subject_id);
/**
 * Provide the language intelligence workspace folder set detail operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_workspace_folder_set_detail(
    UmiLanguageIntelligenceWorkspaceFolder *value,
    const char *detail);
/**
 * Provide the language intelligence workspace folder touch operation used by this module
 * and its client applications.
 */
void umi_language_intelligence_workspace_folder_touch(UmiLanguageIntelligenceWorkspaceFolder *value);
/**
 * Provide the language intelligence workspace folder same identity operation used by this
 * module and its client applications.
 */
int umi_language_intelligence_workspace_folder_same_identity(
    const UmiLanguageIntelligenceWorkspaceFolder *left,
    const UmiLanguageIntelligenceWorkspaceFolder *right);

#ifdef __cplusplus
}
#endif
#endif
