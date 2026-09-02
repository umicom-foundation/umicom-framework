/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/notebook_document.c
 *
 * PURPOSE:
 *   Implement represent notebook-style documents without tying the core to one editor toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/notebook_document.h"

#include <string.h>

/*
 * Initialise language intelligence notebook document from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_notebook_document_init(
    UmiLanguageIntelligenceNotebookDocument *value,
    const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_LANGUAGE_INTELLIGENCE_NOTEBOOK_DOCUMENT_API_VERSION;
    value->enabled = 1;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) {
        (void)umi_language_intelligence_copy_text(
            value->id, sizeof(value->id), id);
    }
}

/*
 * Check that language intelligence notebook document satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_language_intelligence_notebook_document_validate(
    const UmiLanguageIntelligenceNotebookDocument *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_LANGUAGE_INTELLIGENCE_NOTEBOOK_DOCUMENT_API_VERSION ||
        !umi_language_intelligence_text_is_valid(value->id) ||
        !umi_language_intelligence_range_is_valid(&value->range)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the language intelligence notebook document set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_notebook_document_set_subject(
    UmiLanguageIntelligenceNotebookDocument *value,
    const char *subject_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || subject_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_language_intelligence_copy_text(
        value->subject_id, sizeof(value->subject_id), subject_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) umi_language_intelligence_notebook_document_touch(value);
    return status;
}

/*
 * Provide the language intelligence notebook document set detail operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_notebook_document_set_detail(
    UmiLanguageIntelligenceNotebookDocument *value,
    const char *detail)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_language_intelligence_copy_text(
        value->detail, sizeof(value->detail), detail);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) umi_language_intelligence_notebook_document_touch(value);
    return status;
}

/*
 * Provide the language intelligence notebook document touch operation used by this module
 * and its client applications.
 */
void umi_language_intelligence_notebook_document_touch(UmiLanguageIntelligenceNotebookDocument *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->revision != UINT64_MAX) value->revision += 1U;
}

/*
 * Provide the language intelligence notebook document same identity operation used by this
 * module and its client applications.
 */
int umi_language_intelligence_notebook_document_same_identity(
    const UmiLanguageIntelligenceNotebookDocument *left,
    const UmiLanguageIntelligenceNotebookDocument *right)
{
    return left != NULL && right != NULL &&
        strcmp(left->id, right->id) == 0;
}
