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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_NOTEBOOK_DOCUMENT_H
#define UMICOM_LANGUAGE_INTELLIGENCE_NOTEBOOK_DOCUMENT_H

#include "umicom/language/intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INTELLIGENCE_NOTEBOOK_DOCUMENT_API_VERSION 1U

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

void umi_language_intelligence_notebook_document_init(
    UmiLanguageIntelligenceNotebookDocument *value,
    const char *id);
UmiStatus umi_language_intelligence_notebook_document_validate(
    const UmiLanguageIntelligenceNotebookDocument *value);
UmiStatus umi_language_intelligence_notebook_document_set_subject(
    UmiLanguageIntelligenceNotebookDocument *value,
    const char *subject_id);
UmiStatus umi_language_intelligence_notebook_document_set_detail(
    UmiLanguageIntelligenceNotebookDocument *value,
    const char *detail);
void umi_language_intelligence_notebook_document_touch(UmiLanguageIntelligenceNotebookDocument *value);
int umi_language_intelligence_notebook_document_same_identity(
    const UmiLanguageIntelligenceNotebookDocument *left,
    const UmiLanguageIntelligenceNotebookDocument *right);

#ifdef __cplusplus
}
#endif
#endif
