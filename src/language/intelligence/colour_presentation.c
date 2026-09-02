/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/colour_presentation.c
 *
 * PURPOSE:
 *   Implement represent provider-neutral colour presentation edits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/colour_presentation.h"

#include <string.h>

/*
 * Initialise language intelligence colour presentation from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_colour_presentation_init(
    UmiLanguageIntelligenceColourPresentation *value,
    const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_LANGUAGE_INTELLIGENCE_COLOUR_PRESENTATION_API_VERSION;
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
 * Check that language intelligence colour presentation satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_language_intelligence_colour_presentation_validate(
    const UmiLanguageIntelligenceColourPresentation *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_LANGUAGE_INTELLIGENCE_COLOUR_PRESENTATION_API_VERSION ||
        !umi_language_intelligence_text_is_valid(value->id) ||
        !umi_language_intelligence_range_is_valid(&value->range)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the language intelligence colour presentation set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_colour_presentation_set_subject(
    UmiLanguageIntelligenceColourPresentation *value,
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
    if (status == UMI_STATUS_OK) umi_language_intelligence_colour_presentation_touch(value);
    return status;
}

/*
 * Provide the language intelligence colour presentation set detail operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_colour_presentation_set_detail(
    UmiLanguageIntelligenceColourPresentation *value,
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
    if (status == UMI_STATUS_OK) umi_language_intelligence_colour_presentation_touch(value);
    return status;
}

/*
 * Provide the language intelligence colour presentation touch operation used by this
 * module and its client applications.
 */
void umi_language_intelligence_colour_presentation_touch(UmiLanguageIntelligenceColourPresentation *value)
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
 * Provide the language intelligence colour presentation same identity operation used by
 * this module and its client applications.
 */
int umi_language_intelligence_colour_presentation_same_identity(
    const UmiLanguageIntelligenceColourPresentation *left,
    const UmiLanguageIntelligenceColourPresentation *right)
{
    return left != NULL && right != NULL &&
        strcmp(left->id, right->id) == 0;
}
