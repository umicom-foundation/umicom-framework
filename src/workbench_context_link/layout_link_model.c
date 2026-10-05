/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/layout_link_model.c
 *
 * PURPOSE:
 *   Implement validation, copying, hashing and mutation for the layout link summary model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/layout_link_model.h"
#include "../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise workbench context link layout link model from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_layout_link_model_init(UmiWorkbenchContextLinkLayoutLinkModel *record,
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
            record->model_id, sizeof(record->model_id), identity);
    }
}

/*
 * Check that workbench context link layout link model satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_link_layout_link_model_validate(
    const UmiWorkbenchContextLinkLayoutLinkModel *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->model_id, '\0', sizeof(record->model_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->layout_id, '\0', sizeof(record->layout_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
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
            record->model_id, sizeof(record->model_id)) ||
        record->model_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_context_link_text_is_valid(
            record->layout_id, sizeof(record->layout_id)) ||
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
 * Copy workbench context link layout link model into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_layout_link_model_copy(
    UmiWorkbenchContextLinkLayoutLinkModel *destination,
    const UmiWorkbenchContextLinkLayoutLinkModel *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_workbench_context_link_layout_link_model_validate(source) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *destination = *source;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context link layout link model hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_link_layout_link_model_hash(
    const UmiWorkbenchContextLinkLayoutLinkModel *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0U;
    hash = umi_workbench_context_link_hash_text(
        hash, record->model_id, sizeof(record->model_id));
    hash = umi_workbench_context_link_hash_text(
        hash, record->layout_id, sizeof(record->layout_id));
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
 * Provide the workbench context link layout link model set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_layout_link_model_set_primary(
    UmiWorkbenchContextLinkLayoutLinkModel *record,
    const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_copy_text(
        record->layout_id, sizeof(record->layout_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

/*
 * Provide the workbench context link layout link model set secondary operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_link_layout_link_model_set_secondary(
    UmiWorkbenchContextLinkLayoutLinkModel *record,
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

/*
 * Provide the workbench context link layout link model touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_layout_link_model_touch(
    UmiWorkbenchContextLinkLayoutLinkModel *record,
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
static uint64_t UmiWorkbenchContextLinkLayoutLinkModelArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x271c61aee307be74);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextLinkLayoutLinkModel *)0)->model_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextLinkLayoutLinkModel *)0)->layout_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextLinkLayoutLinkModel *)0)->group_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWorkbenchContextLinkLayoutLinkModelArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWorkbenchContextLinkLayoutLinkModel *)0)->model_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextLinkLayoutLinkModel *)0)->layout_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextLinkLayoutLinkModel *)0)->group_id) - 1U +
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
static void UmiWorkbenchContextLinkLayoutLinkModelArchiveWrite(UmiArchiveWriter *writer, const UmiWorkbenchContextLinkLayoutLinkModel *value)
{
    UmiArchiveWriteText(writer, value->model_id, sizeof(value->model_id));
    UmiArchiveWriteText(writer, value->layout_id, sizeof(value->layout_id));
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
static void UmiWorkbenchContextLinkLayoutLinkModelArchiveRead(UmiArchiveReader *reader, UmiWorkbenchContextLinkLayoutLinkModel *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->model_id, sizeof(value->model_id));
    UmiArchiveReadText(reader, value->layout_id, sizeof(value->layout_id));
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
static UmiStatus UmiWorkbenchContextLinkLayoutLinkModelArchiveValidate(const UmiWorkbenchContextLinkLayoutLinkModel *value)
{
    return umi_workbench_context_link_layout_link_model_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_workbench_context_link_layout_link_model_archive_encode, umi_workbench_context_link_layout_link_model_archive_decode,
    UmiWorkbenchContextLinkLayoutLinkModel, UmiWorkbenchContextLinkLayoutLinkModelArchiveSchema, UmiWorkbenchContextLinkLayoutLinkModelArchiveBound, UmiWorkbenchContextLinkLayoutLinkModelArchiveWrite, UmiWorkbenchContextLinkLayoutLinkModelArchiveRead, UmiWorkbenchContextLinkLayoutLinkModelArchiveValidate)
