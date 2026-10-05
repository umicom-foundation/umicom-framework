/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_workbench_snapshot.c
 *
 * PURPOSE:
 *   Capture the aggregate debugger workbench selection and visible-state summary.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/workbench/debug_workbench_snapshot.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Initialise debug workbench debug workbench snapshot from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_debug_workbench_snapshot_init(UmiDebugWorkbenchDebugWorkbenchSnapshot *model)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model != NULL) { memset(model, 0, sizeof *model); model->generation = 1U; } }

/*
 * Provide the debug workbench debug workbench snapshot capture operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_workbench_snapshot_capture(UmiDebugWorkbenchDebugWorkbenchSnapshot *model, const char *session_id, const char *item_id, uint32_t sessions, uint32_t breakpoints, uint32_t threads, uint32_t watches)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !umi_debug_workbench_id_valid(session_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_debug_workbench_copy_text(model->active_session_id, sizeof model->active_session_id, session_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item_id != NULL && item_id[0] != '\0' && umi_debug_workbench_copy_text(model->active_item_id, sizeof model->active_item_id, item_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    model->session_count = sessions; model->breakpoint_count = breakpoints; model->thread_count = threads; model->watch_count = watches; model->generation++;
    return UMI_STATUS_OK;
}

/*
 * Check that debug workbench debug workbench snapshot satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_debug_workbench_snapshot_valid(const UmiDebugWorkbenchDebugWorkbenchSnapshot *model)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (model == NULL) return 0;
    if (memchr(model->active_session_id, '\0', sizeof(model->active_session_id)) == NULL) return 0;
    if (memchr(model->active_item_id, '\0', sizeof(model->active_item_id)) == NULL) return 0;
 return model != NULL && model->generation > 0U && (model->active_session_id[0] == '\0' || umi_debug_workbench_id_valid(model->active_session_id)); }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDebugWorkbenchDebugWorkbenchSnapshotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x380b65fdf93aee57);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugWorkbenchDebugWorkbenchSnapshot *)0)->active_session_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugWorkbenchDebugWorkbenchSnapshot *)0)->active_item_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDebugWorkbenchDebugWorkbenchSnapshotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDebugWorkbenchDebugWorkbenchSnapshot *)0)->active_session_id) - 1U +
        8U + sizeof(((UmiDebugWorkbenchDebugWorkbenchSnapshot *)0)->active_item_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDebugWorkbenchDebugWorkbenchSnapshotArchiveWrite(UmiArchiveWriter *writer, const UmiDebugWorkbenchDebugWorkbenchSnapshot *value)
{
    UmiArchiveWriteText(writer, value->active_session_id, sizeof(value->active_session_id));
    UmiArchiveWriteText(writer, value->active_item_id, sizeof(value->active_item_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->session_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->breakpoint_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->thread_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->watch_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->generation);
}
static void UmiDebugWorkbenchDebugWorkbenchSnapshotArchiveRead(UmiArchiveReader *reader, UmiDebugWorkbenchDebugWorkbenchSnapshot *value)
{
    UmiArchiveReadText(reader, value->active_session_id, sizeof(value->active_session_id));
    UmiArchiveReadText(reader, value->active_item_id, sizeof(value->active_item_id));
    value->session_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->breakpoint_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->thread_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->watch_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->generation = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiDebugWorkbenchDebugWorkbenchSnapshotArchiveValidate(const UmiDebugWorkbenchDebugWorkbenchSnapshot *value)
{
    return umi_debug_workbench_debug_workbench_snapshot_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_debug_workbench_debug_workbench_snapshot_archive_encode, umi_debug_workbench_debug_workbench_snapshot_archive_decode,
    UmiDebugWorkbenchDebugWorkbenchSnapshot, UmiDebugWorkbenchDebugWorkbenchSnapshotArchiveSchema, UmiDebugWorkbenchDebugWorkbenchSnapshotArchiveBound, UmiDebugWorkbenchDebugWorkbenchSnapshotArchiveWrite, UmiDebugWorkbenchDebugWorkbenchSnapshotArchiveRead, UmiDebugWorkbenchDebugWorkbenchSnapshotArchiveValidate)
