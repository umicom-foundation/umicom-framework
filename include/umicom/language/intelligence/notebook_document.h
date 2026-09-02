/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/notebook_document.h
 *
 * PURPOSE:
 *   Represent notebook-style documents without tying the core to one editor toolkit.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_NOTEBOOK_DOCUMENT_H
#define UMICOM_LANGUAGE_INTELLIGENCE_NOTEBOOK_DOCUMENT_H

#include "umicom/language/intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INTELLIGENCE_NOTEBOOK_DOCUMENT_API_VERSION 1U

/**
 * Represent the language intelligence notebook document data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceNotebookDocument {
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
} UmiLanguageIntelligenceNotebookDocument;

/**
 * Initialise language intelligence notebook document from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_notebook_document_init(
    UmiLanguageIntelligenceNotebookDocument *value,
    const char *id);
/**
 * Check that language intelligence notebook document satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_language_intelligence_notebook_document_validate(
    const UmiLanguageIntelligenceNotebookDocument *value);
/**
 * Provide the language intelligence notebook document set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_notebook_document_set_subject(
    UmiLanguageIntelligenceNotebookDocument *value,
    const char *subject_id);
/**
 * Provide the language intelligence notebook document set detail operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_notebook_document_set_detail(
    UmiLanguageIntelligenceNotebookDocument *value,
    const char *detail);
/**
 * Provide the language intelligence notebook document touch operation used by this module
 * and its client applications.
 */
void umi_language_intelligence_notebook_document_touch(UmiLanguageIntelligenceNotebookDocument *value);
/**
 * Provide the language intelligence notebook document same identity operation used by this
 * module and its client applications.
 */
int umi_language_intelligence_notebook_document_same_identity(
    const UmiLanguageIntelligenceNotebookDocument *left,
    const UmiLanguageIntelligenceNotebookDocument *right);

#ifdef __cplusplus
}
#endif
#endif
