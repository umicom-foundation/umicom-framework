/* Umicom Foundation | Sammy Hegab | MIT
 * Public-header client. No source-tree includes or private queue structure. */
#include "umicom/platform/task_queue.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
static UmiStatus Count(UmiTaskContext *context, void *data)
{
    (void)context;
    ++*(unsigned *)data;
    return UMI_STATUS_OK;
}
int main(void)
{
    unsigned completed = 0;
    UmiTask *task = NULL;
    UmiTaskQueue *queue = NULL;
    UmiTaskQueueConfig settings = {1U, 2U};
    UmiTaskConfig item = {"Inspect workshop note", Count, &completed, NULL, NULL};
    UmiTaskQueueShutdownSnapshot snapshot;
    UmiStatus status;
    int failed = 1;
    if (!UmiThreadCanTryJoin()) return 77;
    if (umi_task_queue_create(&settings, &queue) != UMI_STATUS_OK) goto cleanup;
    if (umi_task_create(&item, &task) != UMI_STATUS_OK) goto cleanup;
    if (umi_task_queue_submit(queue, task) != UMI_STATUS_OK) goto cleanup;
    if (UmiTaskQueueRequestShutdown(queue, 0, 0) != UMI_STATUS_OK) goto cleanup;
    for (unsigned attempt = 0U; attempt < 5000U; ++attempt) {
        status = UmiTaskQueueTryFinishShutdown(queue);
        if (status == UMI_STATUS_OK) break;
        if (status != UMI_STATUS_BUSY) goto cleanup;
        umi_thread_sleep_ms(1U);
    }
    if (UmiTaskQueueCaptureShutdown(queue, &snapshot) != UMI_STATUS_OK ||
        snapshot.phase != UMI_TASK_QUEUE_STOPPED || completed != 1U) goto cleanup;
    if (UmiTaskQueueReleaseStopped(&queue) != UMI_STATUS_OK || queue != NULL) goto cleanup;
    printf("Installed client: %s; one task, joined workers, released queue.\n", umi_status_text(UMI_STATUS_OK));
    failed = 0;
cleanup:
    if (queue != NULL) {
        (void)UmiTaskQueueRequestShutdown(queue, 1, 1);
        if (umi_task_queue_shutdown(queue, 1) == UMI_STATUS_OK)
            (void)UmiTaskQueueReleaseStopped(&queue);
    }
    umi_task_destroy(task);
    return failed;
}
