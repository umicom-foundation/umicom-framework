/*-----------------------------------------------------------------------------
 * Umicom Framework terminal/process/task metric projection.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/metrics.h"

#include <string.h>

UmiStatus umi_terminal_metrics_collect(UmiTerminalManager *manager,
                                       UmiProcessSupervisor *supervisor,
                                       UmiTaskQueue *task_queue,
                                       UmiTerminalMetrics *out_metrics)
{
    size_t index;
    if (manager == NULL || supervisor == NULL || task_queue == NULL ||
        out_metrics == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_metrics, 0, sizeof(*out_metrics));
    out_metrics->sessions = umi_terminal_manager_count(manager);
    for (index = 0U; index < out_metrics->sessions; ++index) {
        UmiTerminalSessionSnapshot snapshot;
        UmiTerminalSession *session = umi_terminal_manager_at(manager, index);
        if (umi_terminal_session_snapshot(session, &snapshot) != UMI_STATUS_OK) {
            continue;
        }
        out_metrics->ready_sessions += snapshot.state == UMI_TERMINAL_READY ? 1U : 0U;
        out_metrics->running_sessions += snapshot.state == UMI_TERMINAL_RUNNING ? 1U : 0U;
        out_metrics->failed_sessions += snapshot.state == UMI_TERMINAL_FAILED ? 1U : 0U;
        out_metrics->commands_executed += snapshot.commands_executed;
        out_metrics->transcript_lines += snapshot.transcript_lines;
    }
    out_metrics->processes = umi_process_supervisor_stats(supervisor);
    out_metrics->tasks = umi_task_queue_stats(task_queue);
    return UMI_STATUS_OK;
}
