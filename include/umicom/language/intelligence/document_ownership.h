/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/document_ownership.h
 *
 * PURPOSE:
 *   Record authoritative language-service ownership for an open document.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_DOCUMENT_OWNERSHIP_H
#define UMICOM_LANGUAGE_INTELLIGENCE_DOCUMENT_OWNERSHIP_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_DOCUMENT_OWNERSHIP_API_VERSION 1U

/**
 * Represent the language intelligence document ownership data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceDocumentOwnership {
    uint32_t struct_size;
    uint32_t api_version;
    char source_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char target_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char scope_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    uint32_t priority;
    int enabled;
    uint64_t revision;
} UmiLanguageIntelligenceDocumentOwnership;

/**
 * Initialise language intelligence document ownership from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_document_ownership_init(UmiLanguageIntelligenceDocumentOwnership *mapping);
/**
 * Copy language intelligence document ownership into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_language_intelligence_document_ownership_set(
    UmiLanguageIntelligenceDocumentOwnership *mapping,
    const char *source_id,
    const char *target_id,
    const char *scope_id);
/**
 * Check that language intelligence document ownership satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_language_intelligence_document_ownership_validate(
    const UmiLanguageIntelligenceDocumentOwnership *mapping);
/**
 * Provide the language intelligence document ownership matches operation used by this
 * module and its client applications.
 */
int umi_language_intelligence_document_ownership_matches(
    const UmiLanguageIntelligenceDocumentOwnership *mapping,
    const char *source_id,
    const char *scope_id);
#ifdef __cplusplus
}
#endif
#endif
