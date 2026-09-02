/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal_ui/process_view.c
 *
 * PURPOSE:
 *   Implement the process view behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework supervised-process view projection.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "umicom/terminal_ui/process_view.h"
#include "view_internal.h"

#include <stdio.h>

/*
 * Initialise terminal ui process view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_terminal_ui_process_view_create(const char *view_id,
                                              UmiTerminalController *controller,
                                              UmiUiViewModel **out_view)
{
    UmiProcessSupervisor *supervisor;
    UmiProcessSupervisorStats stats;
    size_t count;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    supervisor = umi_terminal_controller_process_supervisor(controller);
    status = umi_terminal_ui_base_view(view_id, "processes", "Processes",
        "Framework-supervised child processes with state, duration and output.", out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    stats = umi_process_supervisor_stats(supervisor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "process.total", (int64_t)stats.jobs);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "process.running", (int64_t)stats.running);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "process.succeeded", (int64_t)stats.succeeded);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "process.failed", (int64_t)stats.failed);
    count = stats.jobs < UMI_TERMINAL_UI_PROCESS_ROWS
        ? stats.jobs : UMI_TERMINAL_UI_PROCESS_ROWS;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "process.row-count", (int64_t)count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiProcessJobSnapshot job;
        char key[64];
        char value[UMI_PROCESS_JOB_LABEL_CAPACITY + 128U];
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_process_supervisor_at(supervisor, index, &job) != UMI_STATUS_OK) continue;
        (void)snprintf(key, sizeof(key), "process.row.%zu", index);
        (void)snprintf(value, sizeof(value), "#%llu  %s  %s  exit=%d  %llums",
            (unsigned long long)job.job_id, job.label,
            umi_process_job_state_text(job.state), job.exit_code,
            (unsigned long long)job.duration_ms);
        status = umi_terminal_ui_set_string(*out_view, key, value);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 0U,
        "studio.action.process.refresh", "Refresh", "Refresh process state");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 1U,
        "studio.action.process.cancel", "Cancel", "Cancel the selected process");
    return status;
}
