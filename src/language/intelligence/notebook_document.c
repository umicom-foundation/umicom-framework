/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/notebook_document.c
 *
 * PURPOSE:
 *   Implement represent notebook-style documents without tying the core to one editor toolkit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/notebook_document.h"

#include <string.h>

void umi_language_intelligence_notebook_document_init(
    UmiLanguageIntelligenceNotebookDocument *value,
    const char *id)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_LANGUAGE_INTELLIGENCE_NOTEBOOK_DOCUMENT_API_VERSION;
    value->enabled = 1;
    value->revision = 1U;
    if (id != NULL) {
        (void)umi_language_intelligence_copy_text(
            value->id, sizeof(value->id), id);
    }
}

UmiStatus umi_language_intelligence_notebook_document_validate(
    const UmiLanguageIntelligenceNotebookDocument *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_LANGUAGE_INTELLIGENCE_NOTEBOOK_DOCUMENT_API_VERSION ||
        !umi_language_intelligence_text_is_valid(value->id) ||
        !umi_language_intelligence_range_is_valid(&value->range)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_language_intelligence_notebook_document_set_subject(
    UmiLanguageIntelligenceNotebookDocument *value,
    const char *subject_id)
{
    UmiStatus status;
    if (value == NULL || subject_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_language_intelligence_copy_text(
        value->subject_id, sizeof(value->subject_id), subject_id);
    if (status == UMI_STATUS_OK) umi_language_intelligence_notebook_document_touch(value);
    return status;
}

UmiStatus umi_language_intelligence_notebook_document_set_detail(
    UmiLanguageIntelligenceNotebookDocument *value,
    const char *detail)
{
    UmiStatus status;
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_language_intelligence_copy_text(
        value->detail, sizeof(value->detail), detail);
    if (status == UMI_STATUS_OK) umi_language_intelligence_notebook_document_touch(value);
    return status;
}

void umi_language_intelligence_notebook_document_touch(UmiLanguageIntelligenceNotebookDocument *value)
{
    if (value == NULL) return;
    if (value->revision != UINT64_MAX) value->revision += 1U;
}

int umi_language_intelligence_notebook_document_same_identity(
    const UmiLanguageIntelligenceNotebookDocument *left,
    const UmiLanguageIntelligenceNotebookDocument *right)
{
    return left != NULL && right != NULL &&
        strcmp(left->id, right->id) == 0;
}
