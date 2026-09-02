/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/document_routing.h
 *
 * PURPOSE:
 *   Route a document URI to the Framework-selected provider or language service.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_DOCUMENT_ROUTING_H
#define UMICOM_LANGUAGE_INTELLIGENCE_DOCUMENT_ROUTING_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_DOCUMENT_ROUTING_API_VERSION 1U

/**
 * Represent the language intelligence document routing data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceDocumentRouting {
    uint32_t struct_size;
    uint32_t api_version;
    char source_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char target_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char scope_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    uint32_t priority;
    int enabled;
    uint64_t revision;
} UmiLanguageIntelligenceDocumentRouting;

/**
 * Initialise language intelligence document routing from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_document_routing_init(UmiLanguageIntelligenceDocumentRouting *mapping);
/**
 * Copy language intelligence document routing into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_language_intelligence_document_routing_set(
    UmiLanguageIntelligenceDocumentRouting *mapping,
    const char *source_id,
    const char *target_id,
    const char *scope_id);
/**
 * Check that language intelligence document routing satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_language_intelligence_document_routing_validate(
    const UmiLanguageIntelligenceDocumentRouting *mapping);
/**
 * Provide the language intelligence document routing matches operation used by this module
 * and its client applications.
 */
int umi_language_intelligence_document_routing_matches(
    const UmiLanguageIntelligenceDocumentRouting *mapping,
    const char *source_id,
    const char *scope_id);
#ifdef __cplusplus
}
#endif
#endif
