/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_session_state_model.c
 *
 * PURPOSE:
 *   Track debugger lifecycle state and enforce legal high-level phase transitions.
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
#include "umicom/debug/workbench/debug_session_state_model.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Initialise debug workbench debug session state model from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_session_state_model_init(UmiDebugWorkbenchDebugSessionStateModel *model, const char *session_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !umi_debug_workbench_id_valid(session_id)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(model, 0, sizeof *model);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_debug_workbench_copy_text(model->session_id, sizeof model->session_id, session_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    model->phase = UMI_DEBUG_WORKBENCH_SESSION_IDLE;
    model->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workbench debug session state model transition operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_session_state_model_transition(UmiDebugWorkbenchDebugSessionStateModel *model, UmiDebugWorkbenchSessionPhase phase)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_debug_workbench_debug_session_state_model_valid(model)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_debug_workbench_session_transition_allowed(model->phase, phase)) return UMI_STATUS_INVALID_STATE;
    model->phase = phase;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workbench debug session state model record stop operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_session_state_model_record_stop(UmiDebugWorkbenchDebugSessionStateModel *model)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_debug_workbench_debug_session_state_model_valid(model)) return UMI_STATUS_INVALID_ARGUMENT;
    model->stop_sequence++;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Check that debug workbench debug session state model satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_debug_session_state_model_valid(const UmiDebugWorkbenchDebugSessionStateModel *model)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (model == NULL) return 0;
    if (memchr(model->session_id, '\0', sizeof(model->session_id)) == NULL) return 0;

    return model != NULL && umi_debug_workbench_id_valid(model->session_id) && model->phase >= UMI_DEBUG_WORKBENCH_SESSION_IDLE && model->phase <= UMI_DEBUG_WORKBENCH_SESSION_FAILED && model->revision > 0U;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDebugWorkbenchDebugSessionStateModelArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x2f9d206a1d59e014);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugWorkbenchDebugSessionStateModel *)0)->session_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDebugWorkbenchDebugSessionStateModelArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDebugWorkbenchDebugSessionStateModel *)0)->session_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiDebugWorkbenchDebugSessionStateModelArchiveWrite(UmiArchiveWriter *writer, const UmiDebugWorkbenchDebugSessionStateModel *value)
{
    UmiArchiveWriteText(writer, value->session_id, sizeof(value->session_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->phase);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->stop_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiDebugWorkbenchDebugSessionStateModelArchiveRead(UmiArchiveReader *reader, UmiDebugWorkbenchDebugSessionStateModel *value)
{
    UmiArchiveReadText(reader, value->session_id, sizeof(value->session_id));
    value->phase = (UmiDebugWorkbenchSessionPhase)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->stop_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiDebugWorkbenchDebugSessionStateModelArchiveValidate(const UmiDebugWorkbenchDebugSessionStateModel *value)
{
    return umi_debug_workbench_debug_session_state_model_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_debug_workbench_debug_session_state_model_archive_encode, umi_debug_workbench_debug_session_state_model_archive_decode,
    UmiDebugWorkbenchDebugSessionStateModel, UmiDebugWorkbenchDebugSessionStateModelArchiveSchema, UmiDebugWorkbenchDebugSessionStateModelArchiveBound, UmiDebugWorkbenchDebugSessionStateModelArchiveWrite, UmiDebugWorkbenchDebugSessionStateModelArchiveRead, UmiDebugWorkbenchDebugSessionStateModelArchiveValidate)
