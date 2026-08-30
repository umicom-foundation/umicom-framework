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

void umi_language_intelligence_document_ownership_init(UmiLanguageIntelligenceDocumentOwnership *mapping);
UmiStatus umi_language_intelligence_document_ownership_set(
    UmiLanguageIntelligenceDocumentOwnership *mapping,
    const char *source_id,
    const char *target_id,
    const char *scope_id);
UmiStatus umi_language_intelligence_document_ownership_validate(
    const UmiLanguageIntelligenceDocumentOwnership *mapping);
int umi_language_intelligence_document_ownership_matches(
    const UmiLanguageIntelligenceDocumentOwnership *mapping,
    const char *source_id,
    const char *scope_id);
#ifdef __cplusplus
}
#endif
#endif
