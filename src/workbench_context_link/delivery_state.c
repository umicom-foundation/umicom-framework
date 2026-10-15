/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/delivery_state.c
 *
 * PURPOSE:
 *   Implement validation, copying, hashing and mutation for the delivery runtime state record.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../base/conditional_archive_internal.h"
#include "umicom/workbench_context_link/delivery_state.h"
#include "umicom/base/text.h"
#include "../base/record_update_internal.h"
#include "../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise workbench context link delivery state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_delivery_state_init(UmiWorkbenchContextLinkDeliveryState *record,
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
            record->state_id, sizeof(record->state_id), identity);
    }
}

/*
 * Check that workbench context link delivery state satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_delivery_state_validate(
    const UmiWorkbenchContextLinkDeliveryState *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->state_id, '\0', sizeof(record->state_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->delivery_id, '\0', sizeof(record->delivery_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->group_id, '\0', sizeof(record->group_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || record->structure_size != sizeof(*record)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_context_link_text_is_valid(
            record->state_id, sizeof(record->state_id)) ||
        record->state_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_context_link_text_is_valid(
            record->delivery_id, sizeof(record->delivery_id)) ||
        !umi_workbench_context_link_text_is_valid(
            record->group_id, sizeof(record->group_id))) {
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
 * Copy workbench context link delivery state into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_delivery_state_copy(
    UmiWorkbenchContextLinkDeliveryState *destination,
    const UmiWorkbenchContextLinkDeliveryState *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_workbench_context_link_delivery_state_validate(source) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *destination = *source;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context link delivery state hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_delivery_state_hash(
    const UmiWorkbenchContextLinkDeliveryState *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0U;
    hash = umi_workbench_context_link_hash_text(
        hash, record->state_id, sizeof(record->state_id));
    hash = umi_workbench_context_link_hash_text(
        hash, record->delivery_id, sizeof(record->delivery_id));
    hash = umi_workbench_context_link_hash_text(
        hash, record->group_id, sizeof(record->group_id));
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
 * Provide the workbench context link delivery state set primary operation used by this
 * module and its client applications.
 */
/* Retain the earlier mutation for review. The replacement below publishes
 * accepted text or sequence values without erasing data or wrapping revisions. */
#if 0
UmiStatus umi_workbench_context_link_delivery_state_set_primary(
    UmiWorkbenchContextLinkDeliveryState *record,
    const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_copy_text(
        record->delivery_id, sizeof(record->delivery_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}
#endif
UmiStatus umi_workbench_context_link_delivery_state_set_primary(
    UmiWorkbenchContextLinkDeliveryState *record,
    const char *value)
{
    /* The shared edit validates before writing and handles overlapping text.
     * Its revision guard also prevents an exhausted review token from wrapping. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(record->delivery_id, sizeof(record->delivery_id),
        value, &record->revision);
}

/*
 * Provide the workbench context link delivery state set secondary operation used by this
 * module and its client applications.
 */
/* Retain the earlier mutation for review. The replacement below publishes
 * accepted text or sequence values without erasing data or wrapping revisions. */
#if 0
UmiStatus umi_workbench_context_link_delivery_state_set_secondary(
    UmiWorkbenchContextLinkDeliveryState *record,
    const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_copy_text(
        record->group_id, sizeof(record->group_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}
#endif
UmiStatus umi_workbench_context_link_delivery_state_set_secondary(
    UmiWorkbenchContextLinkDeliveryState *record,
    const char *value)
{
    /* The shared edit validates before writing and handles overlapping text.
     * Its revision guard also prevents an exhausted review token from wrapping. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(record->group_id, sizeof(record->group_id),
        value, &record->revision);
}

/*
 * Provide the workbench context link delivery state touch operation used by this module
 * and its client applications.
 */
/* Retain the earlier mutation for review. The replacement below publishes
 * accepted text or sequence values without erasing data or wrapping revisions. */
#if 0
void umi_workbench_context_link_delivery_state_touch(
    UmiWorkbenchContextLinkDeliveryState *record,
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
void umi_workbench_context_link_delivery_state_touch(
    UmiWorkbenchContextLinkDeliveryState *record,
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
static uint64_t UmiWorkbenchContextLinkDeliveryStateArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xca33999464e09c5a);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextLinkDeliveryState *)0)->state_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextLinkDeliveryState *)0)->delivery_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextLinkDeliveryState *)0)->group_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWorkbenchContextLinkDeliveryStateArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWorkbenchContextLinkDeliveryState *)0)->state_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextLinkDeliveryState *)0)->delivery_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextLinkDeliveryState *)0)->group_id) - 1U +
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
static void UmiWorkbenchContextLinkDeliveryStateArchiveWrite(UmiArchiveWriter *writer, const UmiWorkbenchContextLinkDeliveryState *value)
{
    UmiArchiveWriteText(writer, value->state_id, sizeof(value->state_id));
    UmiArchiveWriteText(writer, value->delivery_id, sizeof(value->delivery_id));
    UmiArchiveWriteText(writer, value->group_id, sizeof(value->group_id));
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
static void UmiWorkbenchContextLinkDeliveryStateArchiveRead(UmiArchiveReader *reader, UmiWorkbenchContextLinkDeliveryState *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->state_id, sizeof(value->state_id));
    UmiArchiveReadText(reader, value->delivery_id, sizeof(value->delivery_id));
    UmiArchiveReadText(reader, value->group_id, sizeof(value->group_id));
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
static UmiStatus UmiWorkbenchContextLinkDeliveryStateArchiveValidate(const UmiWorkbenchContextLinkDeliveryState *value)
{
    return umi_workbench_context_link_delivery_state_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_workbench_context_link_delivery_state_archive_encode, umi_workbench_context_link_delivery_state_archive_decode,
    UmiWorkbenchContextLinkDeliveryState, UmiWorkbenchContextLinkDeliveryStateArchiveSchema, UmiWorkbenchContextLinkDeliveryStateArchiveBound, UmiWorkbenchContextLinkDeliveryStateArchiveWrite, UmiWorkbenchContextLinkDeliveryStateArchiveRead, UmiWorkbenchContextLinkDeliveryStateArchiveValidate)

/* Reviewed drafts belong to one context identity. Validation remains with this
 * owner, and publication advances exactly once without dispatching UI events. */
UMI_DEFINE_REVIEWED_IDENTITY_RECORD_EDIT(umi_workbench_context_link_delivery_state_replace_if_current,
    UmiWorkbenchContextLinkDeliveryState, state_id, umi_workbench_context_link_delivery_state_validate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_workbench_context_link_delivery_state_archive_decode_if_unchanged, umi_workbench_context_link_delivery_state_archive_encode,
    UmiWorkbenchContextLinkDeliveryState, UmiWorkbenchContextLinkDeliveryStateArchiveSchema, UmiWorkbenchContextLinkDeliveryStateArchiveBound, UmiWorkbenchContextLinkDeliveryStateArchiveRead, UmiWorkbenchContextLinkDeliveryStateArchiveValidate)
