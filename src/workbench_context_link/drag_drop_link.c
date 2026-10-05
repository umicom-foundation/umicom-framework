/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/drag_drop_link.c
 *
 * PURPOSE:
 *   Implement validation, copying, hashing and mutation for the drag-and-drop context-link operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/drag_drop_link.h"
#include "../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise workbench context link drag drop link from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_drag_drop_link_init(UmiWorkbenchContextLinkDragDropLink *record,
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
            record->operation_id, sizeof(record->operation_id), identity);
    }
}

/*
 * Check that workbench context link drag drop link satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_drag_drop_link_validate(
    const UmiWorkbenchContextLinkDragDropLink *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->operation_id, '\0', sizeof(record->operation_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->source_panel_id, '\0', sizeof(record->source_panel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->target_panel_id, '\0', sizeof(record->target_panel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || record->structure_size != sizeof(*record)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_context_link_text_is_valid(
            record->operation_id, sizeof(record->operation_id)) ||
        record->operation_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_context_link_text_is_valid(
            record->source_panel_id, sizeof(record->source_panel_id)) ||
        !umi_workbench_context_link_text_is_valid(
            record->target_panel_id, sizeof(record->target_panel_id))) {
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
 * Copy workbench context link drag drop link into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_drag_drop_link_copy(
    UmiWorkbenchContextLinkDragDropLink *destination,
    const UmiWorkbenchContextLinkDragDropLink *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_workbench_context_link_drag_drop_link_validate(source) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *destination = *source;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context link drag drop link hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_drag_drop_link_hash(
    const UmiWorkbenchContextLinkDragDropLink *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0U;
    hash = umi_workbench_context_link_hash_text(
        hash, record->operation_id, sizeof(record->operation_id));
    hash = umi_workbench_context_link_hash_text(
        hash, record->source_panel_id, sizeof(record->source_panel_id));
    hash = umi_workbench_context_link_hash_text(
        hash, record->target_panel_id, sizeof(record->target_panel_id));
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
 * Provide the workbench context link drag drop link set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_drag_drop_link_set_primary(
    UmiWorkbenchContextLinkDragDropLink *record,
    const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_copy_text(
        record->source_panel_id, sizeof(record->source_panel_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

/*
 * Provide the workbench context link drag drop link set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_drag_drop_link_set_secondary(
    UmiWorkbenchContextLinkDragDropLink *record,
    const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_copy_text(
        record->target_panel_id, sizeof(record->target_panel_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

/*
 * Provide the workbench context link drag drop link touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_drag_drop_link_touch(
    UmiWorkbenchContextLinkDragDropLink *record,
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

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiWorkbenchContextLinkDragDropLinkArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4a54614e0a065fd9);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextLinkDragDropLink *)0)->operation_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextLinkDragDropLink *)0)->source_panel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextLinkDragDropLink *)0)->target_panel_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWorkbenchContextLinkDragDropLinkArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWorkbenchContextLinkDragDropLink *)0)->operation_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextLinkDragDropLink *)0)->source_panel_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextLinkDragDropLink *)0)->target_panel_id) - 1U +
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
static void UmiWorkbenchContextLinkDragDropLinkArchiveWrite(UmiArchiveWriter *writer, const UmiWorkbenchContextLinkDragDropLink *value)
{
    UmiArchiveWriteText(writer, value->operation_id, sizeof(value->operation_id));
    UmiArchiveWriteText(writer, value->source_panel_id, sizeof(value->source_panel_id));
    UmiArchiveWriteText(writer, value->target_panel_id, sizeof(value->target_panel_id));
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
static void UmiWorkbenchContextLinkDragDropLinkArchiveRead(UmiArchiveReader *reader, UmiWorkbenchContextLinkDragDropLink *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->operation_id, sizeof(value->operation_id));
    UmiArchiveReadText(reader, value->source_panel_id, sizeof(value->source_panel_id));
    UmiArchiveReadText(reader, value->target_panel_id, sizeof(value->target_panel_id));
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
static UmiStatus UmiWorkbenchContextLinkDragDropLinkArchiveValidate(const UmiWorkbenchContextLinkDragDropLink *value)
{
    return umi_workbench_context_link_drag_drop_link_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_workbench_context_link_drag_drop_link_archive_encode, umi_workbench_context_link_drag_drop_link_archive_decode,
    UmiWorkbenchContextLinkDragDropLink, UmiWorkbenchContextLinkDragDropLinkArchiveSchema, UmiWorkbenchContextLinkDragDropLinkArchiveBound, UmiWorkbenchContextLinkDragDropLinkArchiveWrite, UmiWorkbenchContextLinkDragDropLinkArchiveRead, UmiWorkbenchContextLinkDragDropLinkArchiveValidate)
