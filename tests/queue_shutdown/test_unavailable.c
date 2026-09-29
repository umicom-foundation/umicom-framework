/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Compile the real implementation with its nonblocking adapter disabled. */
#include "umicom/platform/task_queue.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); return 1; } } while (0)
static int Work(void *data) { (void)data; return 8; }
int main(void)
{
    UmiThread *thread = NULL;
    UmiTaskQueue *queue = NULL;
    UmiTaskQueueShutdownSnapshot snapshot;
    UmiTaskQueueConfig config = {1U, 2U};
    int result = 0;
    CHECK(UmiThreadCanTryJoin() == 0);
    CHECK(umi_thread_start(Work, NULL, &thread) == UMI_STATUS_OK);
    CHECK(UmiThreadTryJoin(thread) == UMI_STATUS_NOT_IMPLEMENTED);
    CHECK(umi_thread_join(thread, &result) == UMI_STATUS_OK && result == 8);
    CHECK(UmiThreadRelease(&thread) == UMI_STATUS_OK);
    CHECK(umi_task_queue_create(&config, &queue) == UMI_STATUS_OK);
    CHECK(UmiTaskQueueRequestShutdown(queue, 0, 0) == UMI_STATUS_OK);
    CHECK(UmiTaskQueueTryFinishShutdown(queue) == UMI_STATUS_NOT_IMPLEMENTED);
    CHECK(UmiTaskQueueCaptureShutdown(queue, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.phase == UMI_TASK_QUEUE_STOP_REQUESTED && !snapshot.nonblockingJoinAvailable);
    CHECK(umi_task_queue_shutdown(queue, 0) == UMI_STATUS_OK);
    CHECK(UmiTaskQueueReleaseStopped(&queue) == UMI_STATUS_OK && queue == NULL);
    puts("PASS unavailable: no hidden blocking fallback");
    return 0;
}
