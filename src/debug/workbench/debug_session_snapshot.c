/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_session_snapshot.c
 *
 * PURPOSE:
 *   Capture durable high-level debugger session state for workbench restoration.
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
#include "umicom/debug/workbench/debug_session_snapshot.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Initialise debug workbench debug session snapshot from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_debug_session_snapshot_init(UmiDebugWorkbenchDebugSessionSnapshot *model)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model != NULL) { memset(model, 0, sizeof *model); model->generation = 1U; } }

/*
 * Provide the debug workbench debug session snapshot capture operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_debug_session_snapshot_capture(UmiDebugWorkbenchDebugSessionSnapshot *model, const char *session_id, const char *item_id, uint32_t sessions, uint32_t breakpoints, uint32_t threads, uint32_t watches)
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
 * Check that debug workbench debug session snapshot satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_session_snapshot_valid(const UmiDebugWorkbenchDebugSessionSnapshot *model)
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
static uint64_t UmiDebugWorkbenchDebugSessionSnapshotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x9756ccfcc7f9d008);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugWorkbenchDebugSessionSnapshot *)0)->active_session_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugWorkbenchDebugSessionSnapshot *)0)->active_item_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDebugWorkbenchDebugSessionSnapshotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDebugWorkbenchDebugSessionSnapshot *)0)->active_session_id) - 1U +
        8U + sizeof(((UmiDebugWorkbenchDebugSessionSnapshot *)0)->active_item_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDebugWorkbenchDebugSessionSnapshotArchiveWrite(UmiArchiveWriter *writer, const UmiDebugWorkbenchDebugSessionSnapshot *value)
{
    UmiArchiveWriteText(writer, value->active_session_id, sizeof(value->active_session_id));
    UmiArchiveWriteText(writer, value->active_item_id, sizeof(value->active_item_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->session_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->breakpoint_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->thread_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->watch_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->generation);
}
static void UmiDebugWorkbenchDebugSessionSnapshotArchiveRead(UmiArchiveReader *reader, UmiDebugWorkbenchDebugSessionSnapshot *value)
{
    UmiArchiveReadText(reader, value->active_session_id, sizeof(value->active_session_id));
    UmiArchiveReadText(reader, value->active_item_id, sizeof(value->active_item_id));
    value->session_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->breakpoint_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->thread_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->watch_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->generation = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiDebugWorkbenchDebugSessionSnapshotArchiveValidate(const UmiDebugWorkbenchDebugSessionSnapshot *value)
{
    return umi_debug_workbench_debug_session_snapshot_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_debug_workbench_debug_session_snapshot_archive_encode, umi_debug_workbench_debug_session_snapshot_archive_decode,
    UmiDebugWorkbenchDebugSessionSnapshot, UmiDebugWorkbenchDebugSessionSnapshotArchiveSchema, UmiDebugWorkbenchDebugSessionSnapshotArchiveBound, UmiDebugWorkbenchDebugSessionSnapshotArchiveWrite, UmiDebugWorkbenchDebugSessionSnapshotArchiveRead, UmiDebugWorkbenchDebugSessionSnapshotArchiveValidate)
