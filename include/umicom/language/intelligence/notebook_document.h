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

/* Text setters publish a complete field and one revision together. Capacity,
 * invalid-input and exhausted-revision refusals preserve the record. Scalar
 * setters also refuse revision exhaustion. Call these on the value's owner;
 * they do not supply locking or persist changes to a storage service. */

/** Construct the usual default value and report invalid input.
 * A null or empty identity returns INVALID_ARGUMENT. An identity without a
 * terminator in sizeof(value->id) readable bytes returns CAPACITY_EXCEEDED;
 * a shorter C string is read only through its terminator. The existing domain
 * validator checks the staged defaults before publication. Any refusal leaves
 * the destination unchanged. id may refer to the destination's own text.
 * This initializes a new value, resetting its fields and revision to the
 * established defaults; do not use it as a live edit while observers retain
 * that identity. It owns no resources, allocates nothing and performs no I/O.
 * Existing void initialization remains available for compatibility. */
UmiStatus umi_language_intelligence_notebook_document_init_checked(UmiLanguageIntelligenceNotebookDocument *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
