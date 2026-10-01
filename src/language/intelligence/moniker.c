/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/moniker.c
 *
 * PURPOSE:
 *   Implement represent stable symbol monikers for cross-project identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/moniker.h"
#include "umicom/base/text.h"
#include "../../base/record_update_internal.h"

#include <string.h>

/*
 * Initialise language intelligence moniker from caller-provided values so later operations
 * receive a known state.
 */
void umi_language_intelligence_moniker_init(
    UmiLanguageIntelligenceMoniker *value,
    const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_LANGUAGE_INTELLIGENCE_MONIKER_API_VERSION;
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
 * Check that language intelligence moniker satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_language_intelligence_moniker_validate(
    const UmiLanguageIntelligenceMoniker *value)
{
    /* Validate each fixed field before any domain helper treats it as a C
     * string. Add new inline text members here when extending this record. */
    if (value == NULL || memchr(value->id, '\0', sizeof(value->id)) == NULL ||
        memchr(value->subject_id, '\0', sizeof(value->subject_id)) == NULL ||
        memchr(value->detail, '\0', sizeof(value->detail)) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_LANGUAGE_INTELLIGENCE_MONIKER_API_VERSION ||
        !umi_language_intelligence_text_is_valid(value->id) ||
        !umi_language_intelligence_range_is_valid(&value->range)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the language intelligence moniker set subject operation used by this module and
 * its client applications.
 */
/* The shared text publication helper replaces a separate copy and revision increment. It avoids partial edits, overlapping-copy hazards and revision reuse; the previous implementation remains for review. */
#if 0
UmiStatus umi_language_intelligence_moniker_set_subject(
    UmiLanguageIntelligenceMoniker *value,
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
    if (status == UMI_STATUS_OK) umi_language_intelligence_moniker_touch(value);
    return status;
}
#endif
UmiStatus umi_language_intelligence_moniker_set_subject(
    UmiLanguageIntelligenceMoniker *value,
    const char *subject_id)
{
    /* A text edit and its revision are one publication. Framework's shared
     * helper checks capacity before writing and supports text from this field
     * itself. Refused edits leave the complete previous record unchanged. */
    if (value == NULL || subject_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(value->subject_id, sizeof(value->subject_id),
                           subject_id, &value->revision);
}

/*
 * Provide the language intelligence moniker set detail operation used by this module and
 * its client applications.
 */
/* The shared text publication helper replaces a separate copy and revision increment. It avoids partial edits, overlapping-copy hazards and revision reuse; the previous implementation remains for review. */
#if 0
UmiStatus umi_language_intelligence_moniker_set_detail(
    UmiLanguageIntelligenceMoniker *value,
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
    if (status == UMI_STATUS_OK) umi_language_intelligence_moniker_touch(value);
    return status;
}
#endif
UmiStatus umi_language_intelligence_moniker_set_detail(
    UmiLanguageIntelligenceMoniker *value,
    const char *detail)
{
    /* A text edit and its revision are one publication. Framework's shared
     * helper checks capacity before writing and supports text from this field
     * itself. Refused edits leave the complete previous record unchanged. */
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(value->detail, sizeof(value->detail),
                           detail, &value->revision);
}

/*
 * Provide the language intelligence moniker touch operation used by this module and its
 * client applications.
 */
void umi_language_intelligence_moniker_touch(UmiLanguageIntelligenceMoniker *value)
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
 * Provide the language intelligence moniker same identity operation used by this module
 * and its client applications.
 */
/* Bounded identity comparison replaces an unchecked string scan. The
 * prior implementation remains available here for engineering review. */
#if 0
int umi_language_intelligence_moniker_same_identity(
    const UmiLanguageIntelligenceMoniker *left,
    const UmiLanguageIntelligenceMoniker *right)
{
    return left != NULL && right != NULL &&
        strcmp(left->id, right->id) == 0;
}
#endif
int umi_language_intelligence_moniker_same_identity(const UmiLanguageIntelligenceMoniker *left, const UmiLanguageIntelligenceMoniker *right)
{
    /* Do not let an unterminated identity read into neighboring fields. */
    return left != NULL && right != NULL &&
        UmiRecordTextFits(left->id, sizeof(left->id)) &&
        UmiRecordTextFits(right->id, sizeof(right->id)) &&
        strcmp(left->id, right->id) == 0;
}

/* A caller can reject an invalid moniker identity without
 * erasing a previously accepted record. Defaults and domain validation stay
 * with this owner; Framework supplies the common staged publication boundary. */
UMI_DEFINE_CHECKED_RECORD_INIT(umi_language_intelligence_moniker_init_checked,
    UmiLanguageIntelligenceMoniker, umi_language_intelligence_moniker_init, umi_language_intelligence_moniker_validate)
