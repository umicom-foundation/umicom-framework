/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_workflow_instance.c
 *
 * PURPOSE:
 *   Verify approval, completion, rollback and overdue workflow controls.
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
#include "umicom/financial_platform/workflow_instance.h"

int main(void)
{
    UmiFinancialWorkflowInstance workflow;
    UmiFinancialWorkflowTask task = {0};
    umi_financial_workflow_instance_init(&workflow, "wf-1", "payment-1");
    (void)strcpy(task.taskId, "approve");
    (void)strcpy(task.role, "approver");
    task.dueMilliseconds = 2000U;
    task.approvalRequired = 1;
    task.state = UMI_FINANCIAL_WORKFLOW_APPROVAL_PENDING;
    assert(umi_financial_workflow_instance_add_task(&workflow, &task) ==
           UMI_STATUS_OK);
    assert(umi_financial_workflow_instance_complete(&workflow, "approve") ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(umi_financial_workflow_instance_approve(&workflow, "approve") ==
           UMI_STATUS_OK);
    assert(umi_financial_workflow_instance_complete(&workflow, "approve") ==
           UMI_STATUS_OK);
    assert(umi_financial_workflow_instance_completion(&workflow) > 99.9);
    assert(umi_financial_workflow_instance_rollback(&workflow, "approve") ==
           UMI_STATUS_OK);
    return 0;
}
