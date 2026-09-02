/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/request_scheduler.c
 *
 * PURPOSE:
 *   Implement represent scheduled language-intelligence work and its ordering metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/request_scheduler.h"

#include <string.h>

/*
 * Initialise language intelligence request scheduler from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_request_scheduler_init(
    UmiLanguageIntelligenceRequestScheduler *value,
    const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_LANGUAGE_INTELLIGENCE_REQUEST_SCHEDULER_API_VERSION;
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
 * Check that language intelligence request scheduler satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_language_intelligence_request_scheduler_validate(
    const UmiLanguageIntelligenceRequestScheduler *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_LANGUAGE_INTELLIGENCE_REQUEST_SCHEDULER_API_VERSION ||
        !umi_language_intelligence_text_is_valid(value->id) ||
        !umi_language_intelligence_range_is_valid(&value->range)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the language intelligence request scheduler set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_request_scheduler_set_subject(
    UmiLanguageIntelligenceRequestScheduler *value,
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
    if (status == UMI_STATUS_OK) umi_language_intelligence_request_scheduler_touch(value);
    return status;
}

/*
 * Provide the language intelligence request scheduler set detail operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_request_scheduler_set_detail(
    UmiLanguageIntelligenceRequestScheduler *value,
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
    if (status == UMI_STATUS_OK) umi_language_intelligence_request_scheduler_touch(value);
    return status;
}

/*
 * Provide the language intelligence request scheduler touch operation used by this module
 * and its client applications.
 */
void umi_language_intelligence_request_scheduler_touch(UmiLanguageIntelligenceRequestScheduler *value)
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
 * Provide the language intelligence request scheduler same identity operation used by this
 * module and its client applications.
 */
int umi_language_intelligence_request_scheduler_same_identity(
    const UmiLanguageIntelligenceRequestScheduler *left,
    const UmiLanguageIntelligenceRequestScheduler *right)
{
    return left != NULL && right != NULL &&
        strcmp(left->id, right->id) == 0;
}
