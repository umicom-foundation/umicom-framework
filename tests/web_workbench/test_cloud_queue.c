/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_cloud_queue.c
 * PURPOSE: Verify queue filtering and approval-gated destructive operations.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/web/workbench/cloud_queue.h"

int main(void)
{
    UmiWebWorkbenchCloudQueueModel *model = calloc(1U, sizeof(*model));
    UmiWebWorkbenchCloudQueue queue;
    const UmiWebWorkbenchCloudQueue *matches[4U];
    assert(model != NULL);
    umi_web_workbench_cloud_queue_model_init(model, NULL);
    memset(&queue, 0, sizeof(queue));
    assert(umi_web_workbench_copy_text(queue.queue_id, sizeof(queue.queue_id),
        "orders") == UMI_STATUS_OK);
    assert(umi_web_workbench_copy_text(queue.name, sizeof(queue.name),
        "Order Events") == UMI_STATUS_OK);
    queue.kind = UMI_WEB_WORKBENCH_CLOUD_QUEUE;
    queue.visible_messages = 10U;
    queue.encrypted = true;
    assert(umi_web_workbench_cloud_queue_upsert(model, &queue) == UMI_STATUS_OK);
    assert(umi_web_workbench_cloud_queue_query(model, "order",
        UMI_WEB_WORKBENCH_CLOUD_QUEUE, true, matches, 4U) == 1U);
    assert(umi_web_workbench_cloud_queue_remove(model, "orders", false) == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_web_workbench_cloud_queue_remove(model, "orders", true) == UMI_STATUS_OK);
    free(model);
    return 0;
}
