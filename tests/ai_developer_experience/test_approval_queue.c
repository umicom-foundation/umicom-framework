/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_approval_queue.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience approval queue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai_developer_experience/approval_queue.h"

int main(void)
{
    UmiAiDeveloperApprovalQueue *queue = NULL;
    UmiAiDeveloperApprovalRequest item = {0};
    UmiAiDeveloperApprovalRequest copy;

    assert(umi_ai_developer_approval_queue_create(&queue) == UMI_STATUS_OK);
    (void)strcpy(item.approval_id, "approval.1");
    item.state = UMI_AI_DEVELOPER_APPROVAL_PENDING;
    item.kind = UMI_AI_DEVELOPER_APPROVAL_TOOL_CALL;

    assert(umi_ai_developer_approval_queue_add(
        queue, &item) == UMI_STATUS_OK);
    assert(umi_ai_developer_approval_queue_pending_count(queue) == 1U);
    assert(umi_ai_developer_approval_queue_set_state(
        queue, "approval.1",
        UMI_AI_DEVELOPER_APPROVAL_APPROVED) == UMI_STATUS_OK);
    assert(umi_ai_developer_approval_queue_pending_count(queue) == 0U);
    assert(umi_ai_developer_approval_queue_find(
        queue, "approval.1", &copy) == UMI_STATUS_OK);
    assert(copy.state == UMI_AI_DEVELOPER_APPROVAL_APPROVED);

    umi_ai_developer_approval_queue_destroy(queue);
    return 0;
}

