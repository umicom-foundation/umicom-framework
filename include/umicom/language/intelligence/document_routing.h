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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_DOCUMENT_ROUTING_H
#define UMICOM_LANGUAGE_INTELLIGENCE_DOCUMENT_ROUTING_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_DOCUMENT_ROUTING_API_VERSION 1U

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

void umi_language_intelligence_document_routing_init(UmiLanguageIntelligenceDocumentRouting *mapping);
UmiStatus umi_language_intelligence_document_routing_set(
    UmiLanguageIntelligenceDocumentRouting *mapping,
    const char *source_id,
    const char *target_id,
    const char *scope_id);
UmiStatus umi_language_intelligence_document_routing_validate(
    const UmiLanguageIntelligenceDocumentRouting *mapping);
int umi_language_intelligence_document_routing_matches(
    const UmiLanguageIntelligenceDocumentRouting *mapping,
    const char *source_id,
    const char *scope_id);
#ifdef __cplusplus
}
#endif
#endif
