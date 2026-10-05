/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_status_model.c
 *
 * PURPOSE:
 *   Aggregate active session, stop reason and inspection-count status for workbench chrome.
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
#include "umicom/debug/workbench/debug_status_model.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Initialise debug workbench debug status model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_status_model_init(UmiDebugWorkbenchDebugStatusModel *model, const char *session_id)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model == NULL || !umi_debug_workbench_id_valid(session_id)) return UMI_STATUS_INVALID_ARGUMENT; memset(model, 0, sizeof *model); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (umi_debug_workbench_copy_text(model->session_id, sizeof model->session_id, session_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; model->phase = UMI_DEBUG_WORKBENCH_SESSION_IDLE; model->revision = 1U; return UMI_STATUS_OK; }
/*
 * Provide the debug workbench debug status model update operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workbench_debug_status_model_update(UmiDebugWorkbenchDebugStatusModel *model, UmiDebugWorkbenchSessionPhase phase, const char *stop_reason, uint32_t threads, uint32_t frames, uint32_t variables)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model == NULL || phase < UMI_DEBUG_WORKBENCH_SESSION_IDLE || phase > UMI_DEBUG_WORKBENCH_SESSION_FAILED) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (stop_reason != NULL && stop_reason[0] != '\0' && umi_debug_workbench_copy_text(model->stop_reason, sizeof model->stop_reason, stop_reason) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; model->phase = phase; model->thread_count = threads; model->frame_count = frames; model->variable_count = variables; model->revision++; return UMI_STATUS_OK; }
/*
 * Check that debug workbench debug status model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_status_model_valid(const UmiDebugWorkbenchDebugStatusModel *model)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (model == NULL) return 0;
    if (memchr(model->session_id, '\0', sizeof(model->session_id)) == NULL) return 0;
    if (memchr(model->stop_reason, '\0', sizeof(model->stop_reason)) == NULL) return 0;
 return model != NULL && umi_debug_workbench_id_valid(model->session_id) && model->phase >= UMI_DEBUG_WORKBENCH_SESSION_IDLE && model->phase <= UMI_DEBUG_WORKBENCH_SESSION_FAILED && model->revision > 0U; }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDebugWorkbenchDebugStatusModelArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4abec1dba24c81af);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugWorkbenchDebugStatusModel *)0)->session_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugWorkbenchDebugStatusModel *)0)->stop_reason)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDebugWorkbenchDebugStatusModelArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDebugWorkbenchDebugStatusModel *)0)->session_id) - 1U +
        8U + sizeof(((UmiDebugWorkbenchDebugStatusModel *)0)->stop_reason) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDebugWorkbenchDebugStatusModelArchiveWrite(UmiArchiveWriter *writer, const UmiDebugWorkbenchDebugStatusModel *value)
{
    UmiArchiveWriteText(writer, value->session_id, sizeof(value->session_id));
    UmiArchiveWriteText(writer, value->stop_reason, sizeof(value->stop_reason));
    UmiArchiveWriteSigned(writer, (int64_t)value->phase);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->thread_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->frame_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->variable_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiDebugWorkbenchDebugStatusModelArchiveRead(UmiArchiveReader *reader, UmiDebugWorkbenchDebugStatusModel *value)
{
    UmiArchiveReadText(reader, value->session_id, sizeof(value->session_id));
    UmiArchiveReadText(reader, value->stop_reason, sizeof(value->stop_reason));
    value->phase = (UmiDebugWorkbenchSessionPhase)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->thread_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->frame_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->variable_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiDebugWorkbenchDebugStatusModelArchiveValidate(const UmiDebugWorkbenchDebugStatusModel *value)
{
    return umi_debug_workbench_debug_status_model_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_debug_workbench_debug_status_model_archive_encode, umi_debug_workbench_debug_status_model_archive_decode,
    UmiDebugWorkbenchDebugStatusModel, UmiDebugWorkbenchDebugStatusModelArchiveSchema, UmiDebugWorkbenchDebugStatusModelArchiveBound, UmiDebugWorkbenchDebugStatusModelArchiveWrite, UmiDebugWorkbenchDebugStatusModelArchiveRead, UmiDebugWorkbenchDebugStatusModelArchiveValidate)
