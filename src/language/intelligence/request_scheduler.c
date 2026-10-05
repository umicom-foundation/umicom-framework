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
#include "../../base/value_archive_internal.h"
#include "umicom/base/text.h"
#include "../../base/record_update_internal.h"

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
/* The shared text publication helper replaces a separate copy and revision increment. It avoids partial edits, overlapping-copy hazards and revision reuse; the previous implementation remains for review. */
#if 0
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
#endif
UmiStatus umi_language_intelligence_request_scheduler_set_subject(
    UmiLanguageIntelligenceRequestScheduler *value,
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
 * Provide the language intelligence request scheduler set detail operation used by this
 * module and its client applications.
 */
/* The shared text publication helper replaces a separate copy and revision increment. It avoids partial edits, overlapping-copy hazards and revision reuse; the previous implementation remains for review. */
#if 0
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
#endif
UmiStatus umi_language_intelligence_request_scheduler_set_detail(
    UmiLanguageIntelligenceRequestScheduler *value,
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
/* Bounded identity comparison replaces an unchecked string scan. The
 * prior implementation remains available here for engineering review. */
#if 0
int umi_language_intelligence_request_scheduler_same_identity(
    const UmiLanguageIntelligenceRequestScheduler *left,
    const UmiLanguageIntelligenceRequestScheduler *right)
{
    return left != NULL && right != NULL &&
        strcmp(left->id, right->id) == 0;
}
#endif
int umi_language_intelligence_request_scheduler_same_identity(const UmiLanguageIntelligenceRequestScheduler *left, const UmiLanguageIntelligenceRequestScheduler *right)
{
    /* Do not let an unterminated identity read into neighboring fields. */
    return left != NULL && right != NULL &&
        UmiRecordTextFits(left->id, sizeof(left->id)) &&
        UmiRecordTextFits(right->id, sizeof(right->id)) &&
        strcmp(left->id, right->id) == 0;
}

/* A caller can reject an invalid request scheduler identity without
 * erasing a previously accepted record. Defaults and domain validation stay
 * with this owner; Framework supplies the common staged publication boundary. */
UMI_DEFINE_CHECKED_RECORD_INIT(umi_language_intelligence_request_scheduler_init_checked,
    UmiLanguageIntelligenceRequestScheduler, umi_language_intelligence_request_scheduler_init, umi_language_intelligence_request_scheduler_validate)

/* Portable state belongs to the Framework owner. Enumerate fields explicitly
 * so saved bytes contain neither struct padding nor unused text. When adding
 * a field, extend both directions, the schema identity and the domain fixture;
 * incompatible layouts must be migrated deliberately before publication. */
static uint64_t ArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdf8593f1d9968bd3);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageIntelligenceRequestScheduler *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageIntelligenceRequestScheduler *)0)->subject_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageIntelligenceRequestScheduler *)0)->detail)) * UINT64_C(1099511628211);
    return schema;
}
static size_t ArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiLanguageIntelligenceRequestScheduler *)0)->id) - 1U +
        8U + sizeof(((UmiLanguageIntelligenceRequestScheduler *)0)->subject_id) - 1U +
        8U + sizeof(((UmiLanguageIntelligenceRequestScheduler *)0)->detail) - 1U +
        32U +
        8U +
        8U +
        8U +
        8U;
}
static void ArchiveWriteFields(UmiArchiveWriter *writer, const UmiLanguageIntelligenceRequestScheduler *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->subject_id, sizeof(value->subject_id));
    UmiArchiveWriteText(writer, value->detail, sizeof(value->detail));
    UmiArchiveWriteUnsigned(writer, value->range.start.line);
    UmiArchiveWriteUnsigned(writer, value->range.start.character);
    UmiArchiveWriteUnsigned(writer, value->range.end.line);
    UmiArchiveWriteUnsigned(writer, value->range.end.character);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->priority);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->flags);
    UmiArchiveWriteSigned(writer, (int64_t)value->enabled);
}
static void ArchiveReadFields(UmiArchiveReader *reader, UmiLanguageIntelligenceRequestScheduler *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->subject_id, sizeof(value->subject_id));
    UmiArchiveReadText(reader, value->detail, sizeof(value->detail));
    value->range.start.line = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->range.start.character = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->range.end.line = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->range.end.character = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->priority = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->flags = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->enabled = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus ArchiveValidate(const UmiLanguageIntelligenceRequestScheduler *value)
{
    return umi_language_intelligence_request_scheduler_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_language_intelligence_request_scheduler_archive_encode, umi_language_intelligence_request_scheduler_archive_decode,
    UmiLanguageIntelligenceRequestScheduler, ArchiveSchema, ArchiveBound, ArchiveWriteFields, ArchiveReadFields, ArchiveValidate)
