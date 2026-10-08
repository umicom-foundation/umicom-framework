/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/data/job_history.c
 * PURPOSE: Demonstrate explicit job recording and recovery without replaying external work.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/data/job_history.h"
#include <stdio.h>
#include <stdlib.h>
/* Replace the memory constructor with create_sqlite at a private absolute path
 * to retain this evidence across launches. Keep database ownership in the host
 * and join every worker before closing it. */
int main(void)
{
    UmiDataServer *server = NULL;
    UmiJobHistory *history = NULL;
    UmiJobHistorySnapshot *snapshot = calloc(1U, sizeof(*snapshot));
    UmiJobHistoryEntry entry = {0};
    UmiStatus status = snapshot != NULL ? umi_data_server_create_memory(&server) : UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK)
        status = UmiJobHistoryCreate(server, "example.workflow", &history);
    if (status == UMI_STATUS_OK)
        status = UmiJobHistoryBegin(history, "example.preview", "Prepare local preview", 1, &entry);
    if (status == UMI_STATUS_OK)
        status = UmiJobHistoryUpdate(history, entry.id, entry.revision, UMI_JOB_HISTORY_RUNNING, 0,
                                     UMI_STATUS_OK, &entry);
    /* A real host performs its separately authorised operation here, only after
     * the RUNNING checkpoint succeeds. This example has no external action. */
    if (status == UMI_STATUS_OK)
        status = UmiJobHistoryUpdate(history, entry.id, entry.revision, UMI_JOB_HISTORY_SUCCEEDED, 1,
                                     UMI_STATUS_OK, &entry);
    if (status == UMI_STATUS_OK)
        status = UmiJobHistoryCapture(history, snapshot);
    if (status == UMI_STATUS_OK)
        printf("Recorded %zu job; %s\n", snapshot->count, UmiJobHistoryStateText(snapshot->entries[0].state));
    else
        fprintf(stderr, "Job history: %s\n", umi_status_text(status));
    UmiJobHistoryDestroy(history);
    umi_data_server_destroy(server);
    free(snapshot);
    return status == UMI_STATUS_OK ? 0 : 1;
}
