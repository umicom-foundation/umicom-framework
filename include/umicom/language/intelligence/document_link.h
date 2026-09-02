/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/document_link.h
 *
 * PURPOSE:
 *   Represent provider-neutral document links and resolution state.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_DOCUMENT_LINK_H
#define UMICOM_LANGUAGE_INTELLIGENCE_DOCUMENT_LINK_H

#include "umicom/language/intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INTELLIGENCE_DOCUMENT_LINK_API_VERSION 1U

/**
 * Represent the language intelligence document link data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceDocumentLink {
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
} UmiLanguageIntelligenceDocumentLink;

/**
 * Initialise language intelligence document link from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_document_link_init(
    UmiLanguageIntelligenceDocumentLink *value,
    const char *id);
/**
 * Check that language intelligence document link satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_language_intelligence_document_link_validate(
    const UmiLanguageIntelligenceDocumentLink *value);
/**
 * Provide the language intelligence document link set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_document_link_set_subject(
    UmiLanguageIntelligenceDocumentLink *value,
    const char *subject_id);
/**
 * Provide the language intelligence document link set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_intelligence_document_link_set_detail(
    UmiLanguageIntelligenceDocumentLink *value,
    const char *detail);
/**
 * Provide the language intelligence document link touch operation used by this module and
 * its client applications.
 */
void umi_language_intelligence_document_link_touch(UmiLanguageIntelligenceDocumentLink *value);
/**
 * Provide the language intelligence document link same identity operation used by this
 * module and its client applications.
 */
int umi_language_intelligence_document_link_same_identity(
    const UmiLanguageIntelligenceDocumentLink *left,
    const UmiLanguageIntelligenceDocumentLink *right);

#ifdef __cplusplus
}
#endif
#endif
