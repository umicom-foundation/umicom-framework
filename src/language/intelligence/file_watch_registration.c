/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/file_watch_registration.c
 *
 * PURPOSE:
 *   Implement describe Framework-owned watched-file registrations for language providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/file_watch_registration.h"

#include <string.h>

/*
 * Initialise language intelligence file watch registration from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_file_watch_registration_init(
    UmiLanguageIntelligenceFileWatchRegistration *value,
    const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_LANGUAGE_INTELLIGENCE_FILE_WATCH_REGISTRATION_API_VERSION;
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
 * Check that language intelligence file watch registration satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_language_intelligence_file_watch_registration_validate(
    const UmiLanguageIntelligenceFileWatchRegistration *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_LANGUAGE_INTELLIGENCE_FILE_WATCH_REGISTRATION_API_VERSION ||
        !umi_language_intelligence_text_is_valid(value->id) ||
        !umi_language_intelligence_range_is_valid(&value->range)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the language intelligence file watch registration set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_language_intelligence_file_watch_registration_set_subject(
    UmiLanguageIntelligenceFileWatchRegistration *value,
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
    if (status == UMI_STATUS_OK) umi_language_intelligence_file_watch_registration_touch(value);
    return status;
}

/*
 * Provide the language intelligence file watch registration set detail operation used by
 * this module and its client applications.
 */
UmiStatus umi_language_intelligence_file_watch_registration_set_detail(
    UmiLanguageIntelligenceFileWatchRegistration *value,
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
    if (status == UMI_STATUS_OK) umi_language_intelligence_file_watch_registration_touch(value);
    return status;
}

/*
 * Provide the language intelligence file watch registration touch operation used by this
 * module and its client applications.
 */
void umi_language_intelligence_file_watch_registration_touch(UmiLanguageIntelligenceFileWatchRegistration *value)
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
 * Provide the language intelligence file watch registration same identity operation used
 * by this module and its client applications.
 */
int umi_language_intelligence_file_watch_registration_same_identity(
    const UmiLanguageIntelligenceFileWatchRegistration *left,
    const UmiLanguageIntelligenceFileWatchRegistration *right)
{
    return left != NULL && right != NULL &&
        strcmp(left->id, right->id) == 0;
}
