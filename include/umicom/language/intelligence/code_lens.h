/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/code_lens.h
 *
 * PURPOSE:
 *   Represent code-lens commands without toolkit or LSP transport ownership.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_CODE_LENS_H
#define UMICOM_LANGUAGE_INTELLIGENCE_CODE_LENS_H

#include "umicom/language/intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INTELLIGENCE_CODE_LENS_API_VERSION 1U

/**
 * Represent the language intelligence code lens data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageIntelligenceCodeLens {
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
} UmiLanguageIntelligenceCodeLens;

/**
 * Initialise language intelligence code lens from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_code_lens_init(
    UmiLanguageIntelligenceCodeLens *value,
    const char *id);
/**
 * Check that language intelligence code lens satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_language_intelligence_code_lens_validate(
    const UmiLanguageIntelligenceCodeLens *value);
/**
 * Provide the language intelligence code lens set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_intelligence_code_lens_set_subject(
    UmiLanguageIntelligenceCodeLens *value,
    const char *subject_id);
/**
 * Provide the language intelligence code lens set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_intelligence_code_lens_set_detail(
    UmiLanguageIntelligenceCodeLens *value,
    const char *detail);
/**
 * Provide the language intelligence code lens touch operation used by this module and its
 * client applications.
 */
void umi_language_intelligence_code_lens_touch(UmiLanguageIntelligenceCodeLens *value);
/**
 * Provide the language intelligence code lens same identity operation used by this module
 * and its client applications.
 */
int umi_language_intelligence_code_lens_same_identity(
    const UmiLanguageIntelligenceCodeLens *left,
    const UmiLanguageIntelligenceCodeLens *right);

#ifdef __cplusplus
}
#endif
#endif
