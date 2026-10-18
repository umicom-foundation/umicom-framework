/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/filter.c
 *
 * PURPOSE:
 *   Implement validation, copying, hashing and mutation for the context-link filter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../base/conditional_archive_internal.h"
#include "umicom/workbench_context_link/filter.h"
#include "umicom/base/text.h"
#include "../base/record_update_internal.h"
#include "../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise workbench context link filter from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_context_link_filter_init(UmiWorkbenchContextLinkFilter *record,
                                           const char *identity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return;
    memset(record, 0, sizeof(*record));
    record->structure_size = (uint32_t)sizeof(*record);
    record->context_kind = UMI_CONTEXT_KIND_GENERIC;
    record->colour = UMI_CONTEXT_COLOUR_NONE;
    record->mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_NONE;
    record->state = UMI_WORKBENCH_CONTEXT_LINK_STATE_DETACHED;
    record->origin = UMI_WORKBENCH_CONTEXT_LINK_ORIGIN_USER;
    record->priority = UMI_WORKBENCH_CONTEXT_LINK_PRIORITY_NORMAL;
    record->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (identity != NULL) {
        (void)umi_workbench_context_link_copy_text(
            record->filter_id, sizeof(record->filter_id), identity);
    }
}

/*
 * Check that workbench context link filter satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_link_filter_validate(
    const UmiWorkbenchContextLinkFilter *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->filter_id, '\0', sizeof(record->filter_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->query_text, '\0', sizeof(record->query_text)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->application_id, '\0', sizeof(record->application_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || record->structure_size != sizeof(*record)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_context_link_text_is_valid(
            record->filter_id, sizeof(record->filter_id)) ||
        record->filter_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_context_link_text_is_valid(
            record->query_text, sizeof(record->query_text)) ||
        !umi_workbench_context_link_text_is_valid(
            record->application_id, sizeof(record->application_id))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (record->context_kind < UMI_CONTEXT_KIND_GENERIC ||
        record->context_kind > UMI_CONTEXT_KIND_SELECTION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (record->colour < UMI_CONTEXT_COLOUR_NONE ||
        record->colour > UMI_CONTEXT_COLOUR_MAGENTA) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (record->mode < UMI_WORKBENCH_CONTEXT_LINK_MODE_NONE ||
        record->mode > UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Copy workbench context link filter into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiStatus umi_workbench_context_link_filter_copy(
    UmiWorkbenchContextLinkFilter *destination,
    const UmiWorkbenchContextLinkFilter *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_workbench_context_link_filter_validate(source) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *destination = *source;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context link filter hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_link_filter_hash(
    const UmiWorkbenchContextLinkFilter *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0U;
    hash = umi_workbench_context_link_hash_text(
        hash, record->filter_id, sizeof(record->filter_id));
    hash = umi_workbench_context_link_hash_text(
        hash, record->query_text, sizeof(record->query_text));
    hash = umi_workbench_context_link_hash_text(
        hash, record->application_id, sizeof(record->application_id));
    hash = umi_workbench_context_link_hash_bytes(
        hash, &record->context_kind, sizeof(record->context_kind));
    hash = umi_workbench_context_link_hash_bytes(
        hash, &record->colour, sizeof(record->colour));
    hash = umi_workbench_context_link_hash_bytes(
        hash, &record->mode, sizeof(record->mode));
    hash = umi_workbench_context_link_hash_bytes(
        hash, &record->state, sizeof(record->state));
    hash = umi_workbench_context_link_hash_bytes(
        hash, &record->flags, sizeof(record->flags));
    return hash;
}

/*
 * Provide the workbench context link filter set primary operation used by this module and
 * its client applications.
 */
/* Retain the earlier mutation for review. The replacement below publishes
 * accepted text or sequence values without erasing data or wrapping revisions. */
#if 0
UmiStatus umi_workbench_context_link_filter_set_primary(
    UmiWorkbenchContextLinkFilter *record,
    const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_copy_text(
        record->query_text, sizeof(record->query_text), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}
#endif
UmiStatus umi_workbench_context_link_filter_set_primary(
    UmiWorkbenchContextLinkFilter *record,
    const char *value)
{
    /* The shared edit validates before writing and handles overlapping text.
     * Its revision guard also prevents an exhausted review token from wrapping. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(record->query_text, sizeof(record->query_text),
        value, &record->revision);
}

/*
 * Provide the workbench context link filter set secondary operation used by this module
 * and its client applications.
 */
/* Retain the earlier mutation for review. The replacement below publishes
 * accepted text or sequence values without erasing data or wrapping revisions. */
#if 0
UmiStatus umi_workbench_context_link_filter_set_secondary(
    UmiWorkbenchContextLinkFilter *record,
    const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_copy_text(
        record->application_id, sizeof(record->application_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}
#endif
UmiStatus umi_workbench_context_link_filter_set_secondary(
    UmiWorkbenchContextLinkFilter *record,
    const char *value)
{
    /* The shared edit validates before writing and handles overlapping text.
     * Its revision guard also prevents an exhausted review token from wrapping. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(record->application_id, sizeof(record->application_id),
        value, &record->revision);
}

/*
 * Provide the workbench context link filter touch operation used by this module and its
 * client applications.
 */
/* Retain the earlier mutation for review. The replacement below publishes
 * accepted text or sequence values without erasing data or wrapping revisions. */
#if 0
void umi_workbench_context_link_filter_touch(
    UmiWorkbenchContextLinkFilter *record,
    uint64_t sequence,
    uint64_t timestamp_ms)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return;
    record->sequence = sequence;
    record->timestamp_ms = timestamp_ms;
    ++record->revision;
}
#endif
void umi_workbench_context_link_filter_touch(
    UmiWorkbenchContextLinkFilter *record,
    uint64_t sequence,
    uint64_t timestamp_ms)
{
    /* The legacy void API cannot report exhaustion. Leave all fields untouched
     * when its token cannot advance; reviewed replacement returns a status. */
    if (record == NULL || record->revision == UINT64_MAX) return;
    record->sequence = sequence;
    record->timestamp_ms = timestamp_ms;
    ++record->revision;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiWorkbenchContextLinkFilterArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa5da435ab402a771);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextLinkFilter *)0)->filter_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextLinkFilter *)0)->query_text)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextLinkFilter *)0)->application_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWorkbenchContextLinkFilterArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWorkbenchContextLinkFilter *)0)->filter_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextLinkFilter *)0)->query_text) - 1U +
        8U + sizeof(((UmiWorkbenchContextLinkFilter *)0)->application_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiWorkbenchContextLinkFilterArchiveWrite(UmiArchiveWriter *writer, const UmiWorkbenchContextLinkFilter *value)
{
    UmiArchiveWriteText(writer, value->filter_id, sizeof(value->filter_id));
    UmiArchiveWriteText(writer, value->query_text, sizeof(value->query_text));
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->context_kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->colour);
    UmiArchiveWriteSigned(writer, (int64_t)value->mode);
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
    UmiArchiveWriteSigned(writer, (int64_t)value->origin);
    UmiArchiveWriteSigned(writer, (int64_t)value->priority);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->flags);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timestamp_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiWorkbenchContextLinkFilterArchiveRead(UmiArchiveReader *reader, UmiWorkbenchContextLinkFilter *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->filter_id, sizeof(value->filter_id));
    UmiArchiveReadText(reader, value->query_text, sizeof(value->query_text));
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    value->context_kind = (UmiContextKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->colour = (UmiContextChannelColour)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->mode = (UmiWorkbenchContextLinkMode)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->state = (UmiWorkbenchContextLinkState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->origin = (UmiWorkbenchContextLinkOrigin)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->priority = (UmiWorkbenchContextLinkPriority)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->flags = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->timestamp_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiWorkbenchContextLinkFilterArchiveValidate(const UmiWorkbenchContextLinkFilter *value)
{
    return umi_workbench_context_link_filter_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_workbench_context_link_filter_archive_encode, umi_workbench_context_link_filter_archive_decode,
    UmiWorkbenchContextLinkFilter, UmiWorkbenchContextLinkFilterArchiveSchema, UmiWorkbenchContextLinkFilterArchiveBound, UmiWorkbenchContextLinkFilterArchiveWrite, UmiWorkbenchContextLinkFilterArchiveRead, UmiWorkbenchContextLinkFilterArchiveValidate)

/* SDK callers choose staging storage independently of their thread stack.
 * Reuse this owner\'s exact schema, reader and validation before publishing
 * a destination; decoded values remain data, never execution authority. */
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_workbench_context_link_filter_archive_decode_with_workspace, UmiWorkbenchContextLinkFilter,
    UmiWorkbenchContextLinkFilterArchiveSchema, UmiWorkbenchContextLinkFilterArchiveBound, UmiWorkbenchContextLinkFilterArchiveRead, UmiWorkbenchContextLinkFilterArchiveValidate)

/* Reviewed drafts belong to one context identity. Validation remains with this
 * owner, and publication advances exactly once without dispatching UI events. */
UMI_DEFINE_REVIEWED_IDENTITY_RECORD_EDIT(umi_workbench_context_link_filter_replace_if_current,
    UmiWorkbenchContextLinkFilter, filter_id, umi_workbench_context_link_filter_validate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_workbench_context_link_filter_archive_decode_if_unchanged, umi_workbench_context_link_filter_archive_encode,
    UmiWorkbenchContextLinkFilter, UmiWorkbenchContextLinkFilterArchiveSchema, UmiWorkbenchContextLinkFilterArchiveBound, UmiWorkbenchContextLinkFilterArchiveRead, UmiWorkbenchContextLinkFilterArchiveValidate)
