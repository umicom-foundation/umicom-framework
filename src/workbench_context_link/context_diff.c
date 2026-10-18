/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/context_diff.c
 *
 * PURPOSE:
 *   Implement validation, copying, hashing and mutation for the context comparison record.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../base/conditional_archive_internal.h"
#include "umicom/workbench_context_link/context_diff.h"
#include "umicom/base/text.h"
#include "../base/record_update_internal.h"
#include "../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise workbench context link context diff from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_context_diff_init(UmiWorkbenchContextLinkContextDiff *record,
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
            record->diff_id, sizeof(record->diff_id), identity);
    }
}

/*
 * Check that workbench context link context diff satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_context_diff_validate(
    const UmiWorkbenchContextLinkContextDiff *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->diff_id, '\0', sizeof(record->diff_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->left_context_id, '\0', sizeof(record->left_context_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->right_context_id, '\0', sizeof(record->right_context_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || record->structure_size != sizeof(*record)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_context_link_text_is_valid(
            record->diff_id, sizeof(record->diff_id)) ||
        record->diff_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_context_link_text_is_valid(
            record->left_context_id, sizeof(record->left_context_id)) ||
        !umi_workbench_context_link_text_is_valid(
            record->right_context_id, sizeof(record->right_context_id))) {
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
 * Copy workbench context link context diff into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_context_diff_copy(
    UmiWorkbenchContextLinkContextDiff *destination,
    const UmiWorkbenchContextLinkContextDiff *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_workbench_context_link_context_diff_validate(source) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *destination = *source;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context link context diff hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_context_diff_hash(
    const UmiWorkbenchContextLinkContextDiff *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0U;
    hash = umi_workbench_context_link_hash_text(
        hash, record->diff_id, sizeof(record->diff_id));
    hash = umi_workbench_context_link_hash_text(
        hash, record->left_context_id, sizeof(record->left_context_id));
    hash = umi_workbench_context_link_hash_text(
        hash, record->right_context_id, sizeof(record->right_context_id));
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
 * Provide the workbench context link context diff set primary operation used by this
 * module and its client applications.
 */
/* Retain the earlier mutation for review. The replacement below publishes
 * accepted text or sequence values without erasing data or wrapping revisions. */
#if 0
UmiStatus umi_workbench_context_link_context_diff_set_primary(
    UmiWorkbenchContextLinkContextDiff *record,
    const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_copy_text(
        record->left_context_id, sizeof(record->left_context_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}
#endif
UmiStatus umi_workbench_context_link_context_diff_set_primary(
    UmiWorkbenchContextLinkContextDiff *record,
    const char *value)
{
    /* The shared edit validates before writing and handles overlapping text.
     * Its revision guard also prevents an exhausted review token from wrapping. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(record->left_context_id, sizeof(record->left_context_id),
        value, &record->revision);
}

/*
 * Provide the workbench context link context diff set secondary operation used by this
 * module and its client applications.
 */
/* Retain the earlier mutation for review. The replacement below publishes
 * accepted text or sequence values without erasing data or wrapping revisions. */
#if 0
UmiStatus umi_workbench_context_link_context_diff_set_secondary(
    UmiWorkbenchContextLinkContextDiff *record,
    const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_copy_text(
        record->right_context_id, sizeof(record->right_context_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}
#endif
UmiStatus umi_workbench_context_link_context_diff_set_secondary(
    UmiWorkbenchContextLinkContextDiff *record,
    const char *value)
{
    /* The shared edit validates before writing and handles overlapping text.
     * Its revision guard also prevents an exhausted review token from wrapping. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(record->right_context_id, sizeof(record->right_context_id),
        value, &record->revision);
}

/*
 * Provide the workbench context link context diff touch operation used by this module and
 * its client applications.
 */
/* Retain the earlier mutation for review. The replacement below publishes
 * accepted text or sequence values without erasing data or wrapping revisions. */
#if 0
void umi_workbench_context_link_context_diff_touch(
    UmiWorkbenchContextLinkContextDiff *record,
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
void umi_workbench_context_link_context_diff_touch(
    UmiWorkbenchContextLinkContextDiff *record,
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
static uint64_t UmiWorkbenchContextLinkContextDiffArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf5e1a1b178a7c0a2);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextLinkContextDiff *)0)->diff_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextLinkContextDiff *)0)->left_context_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextLinkContextDiff *)0)->right_context_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWorkbenchContextLinkContextDiffArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWorkbenchContextLinkContextDiff *)0)->diff_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextLinkContextDiff *)0)->left_context_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextLinkContextDiff *)0)->right_context_id) - 1U +
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
static void UmiWorkbenchContextLinkContextDiffArchiveWrite(UmiArchiveWriter *writer, const UmiWorkbenchContextLinkContextDiff *value)
{
    UmiArchiveWriteText(writer, value->diff_id, sizeof(value->diff_id));
    UmiArchiveWriteText(writer, value->left_context_id, sizeof(value->left_context_id));
    UmiArchiveWriteText(writer, value->right_context_id, sizeof(value->right_context_id));
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
static void UmiWorkbenchContextLinkContextDiffArchiveRead(UmiArchiveReader *reader, UmiWorkbenchContextLinkContextDiff *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->diff_id, sizeof(value->diff_id));
    UmiArchiveReadText(reader, value->left_context_id, sizeof(value->left_context_id));
    UmiArchiveReadText(reader, value->right_context_id, sizeof(value->right_context_id));
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
static UmiStatus UmiWorkbenchContextLinkContextDiffArchiveValidate(const UmiWorkbenchContextLinkContextDiff *value)
{
    return umi_workbench_context_link_context_diff_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_workbench_context_link_context_diff_archive_encode, umi_workbench_context_link_context_diff_archive_decode,
    UmiWorkbenchContextLinkContextDiff, UmiWorkbenchContextLinkContextDiffArchiveSchema, UmiWorkbenchContextLinkContextDiffArchiveBound, UmiWorkbenchContextLinkContextDiffArchiveWrite, UmiWorkbenchContextLinkContextDiffArchiveRead, UmiWorkbenchContextLinkContextDiffArchiveValidate)

/* SDK callers choose staging storage independently of their thread stack.
 * Reuse this owner\'s exact schema, reader and validation before publishing
 * a destination; decoded values remain data, never execution authority. */
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_workbench_context_link_context_diff_archive_decode_with_workspace, UmiWorkbenchContextLinkContextDiff,
    UmiWorkbenchContextLinkContextDiffArchiveSchema, UmiWorkbenchContextLinkContextDiffArchiveBound, UmiWorkbenchContextLinkContextDiffArchiveRead, UmiWorkbenchContextLinkContextDiffArchiveValidate)

/* Reviewed drafts belong to one context identity. Validation remains with this
 * owner, and publication advances exactly once without dispatching UI events. */
UMI_DEFINE_REVIEWED_IDENTITY_RECORD_EDIT(umi_workbench_context_link_context_diff_replace_if_current,
    UmiWorkbenchContextLinkContextDiff, diff_id, umi_workbench_context_link_context_diff_validate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_workbench_context_link_context_diff_archive_decode_if_unchanged, umi_workbench_context_link_context_diff_archive_encode,
    UmiWorkbenchContextLinkContextDiff, UmiWorkbenchContextLinkContextDiffArchiveSchema, UmiWorkbenchContextLinkContextDiffArchiveBound, UmiWorkbenchContextLinkContextDiffArchiveRead, UmiWorkbenchContextLinkContextDiffArchiveValidate)
