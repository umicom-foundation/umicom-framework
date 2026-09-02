/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_timeline_event.c
 *
 * PURPOSE:
 *   Represent one timestamped debugger lifecycle, stop or navigation event.
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
#include "umicom/debug/workbench/debug_timeline_event.h"
#include <string.h>

/*
 * Initialise debug workbench debug timeline event from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_timeline_event_init(UmiDebugWorkbenchDebugTimelineEvent *model, const char *id, uint64_t sequence, uint64_t timestamp_ns, UmiDebugWorkbenchSessionPhase phase, const char *label)
{
    UmiDebugWorkbenchRange range = {{0U, 0U}, {0U, 0U}};
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || sequence == 0U || phase < UMI_DEBUG_WORKBENCH_SESSION_IDLE || phase > UMI_DEBUG_WORKBENCH_SESSION_FAILED) return UMI_STATUS_INVALID_ARGUMENT;
    memset(model, 0, sizeof *model);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_debug_workbench_entry_init(&model->value, id, label, "", NULL, range) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    model->sequence = sequence; model->timestamp_ns = timestamp_ns; model->phase = phase; model->revision = 1U; return UMI_STATUS_OK;
}
/*
 * Provide the debug workbench debug timeline event retime operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_debug_timeline_event_retime(UmiDebugWorkbenchDebugTimelineEvent *model, uint64_t timestamp_ns)
{ /* Preserve the original failure result so the caller can respond to the correct cause. */ if (!umi_debug_workbench_debug_timeline_event_valid(model)) return UMI_STATUS_INVALID_ARGUMENT; model->timestamp_ns = timestamp_ns; model->revision++; return UMI_STATUS_OK; }
/*
 * Check that debug workbench debug timeline event satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_timeline_event_valid(const UmiDebugWorkbenchDebugTimelineEvent *model)
{ return model != NULL && umi_debug_workbench_entry_valid(&model->value) && model->sequence > 0U && model->phase >= UMI_DEBUG_WORKBENCH_SESSION_IDLE && model->phase <= UMI_DEBUG_WORKBENCH_SESSION_FAILED && model->revision > 0U; }
