/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/workflow_instance.c
 *
 * PURPOSE:
 *   Implement governed financial workflow task and approval transitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/workflow_instance.h"

#include <stdio.h>
#include <string.h>

void umi_financial_workflow_instance_init(
    UmiFinancialWorkflowInstance *workflow,
    const char *workflowId,
    const char *subjectId)
{
    if (workflow == NULL) return;
    (void)memset(workflow, 0, sizeof(*workflow));
    if (workflowId != NULL)
        (void)snprintf(workflow->workflowId, sizeof(workflow->workflowId),
                       "%s", workflowId);
    if (subjectId != NULL)
        (void)snprintf(workflow->subjectId, sizeof(workflow->subjectId),
                       "%s", subjectId);
    workflow->revision = 1U;
    workflow->rollbackAvailable = 1;
}

static UmiFinancialWorkflowTask *find_task(
    UmiFinancialWorkflowInstance *workflow,
    const char *taskId)
{
    size_t i;
    if (workflow == NULL || taskId == NULL) return NULL;
    for (i = 0U; i < workflow->count; ++i)
        if (strcmp(workflow->tasks[i].taskId, taskId) == 0)
            return &workflow->tasks[i];
    return NULL;
}

UmiStatus umi_financial_workflow_instance_add_task(
    UmiFinancialWorkflowInstance *workflow,
    const UmiFinancialWorkflowTask *task)
{
    if (workflow == NULL || task == NULL ||
        workflow->workflowId[0] == '\0' ||
        workflow->subjectId[0] == '\0' ||
        task->taskId[0] == '\0' || task->role[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (find_task(workflow, task->taskId) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (workflow->count >= UMI_FINANCIAL_WORKFLOW_TASK_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    workflow->tasks[workflow->count++] = *task;
    workflow->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_financial_workflow_instance_approve(
    UmiFinancialWorkflowInstance *workflow,
    const char *taskId)
{
    UmiFinancialWorkflowTask *task = find_task(workflow, taskId);
    if (task == NULL) return UMI_STATUS_NOT_FOUND;
    if (!task->approvalRequired) return UMI_STATUS_INVALID_STATE;
    task->approved = 1;
    task->state = UMI_FINANCIAL_WORKFLOW_PENDING;
    workflow->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_financial_workflow_instance_complete(
    UmiFinancialWorkflowInstance *workflow,
    const char *taskId)
{
    UmiFinancialWorkflowTask *task = find_task(workflow, taskId);
    if (task == NULL) return UMI_STATUS_NOT_FOUND;
    if (task->approvalRequired && !task->approved)
        return UMI_STATUS_PERMISSION_DENIED;
    if (task->state == UMI_FINANCIAL_WORKFLOW_REJECTED ||
        task->state == UMI_FINANCIAL_WORKFLOW_ROLLED_BACK)
        return UMI_STATUS_INVALID_STATE;
    task->state = UMI_FINANCIAL_WORKFLOW_COMPLETED;
    workflow->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_financial_workflow_instance_rollback(
    UmiFinancialWorkflowInstance *workflow,
    const char *taskId)
{
    UmiFinancialWorkflowTask *task;
    if (workflow == NULL || !workflow->rollbackAvailable)
        return UMI_STATUS_PERMISSION_DENIED;
    task = find_task(workflow, taskId);
    if (task == NULL) return UMI_STATUS_NOT_FOUND;
    if (task->state != UMI_FINANCIAL_WORKFLOW_COMPLETED)
        return UMI_STATUS_INVALID_STATE;
    task->state = UMI_FINANCIAL_WORKFLOW_ROLLED_BACK;
    workflow->revision += 1U;
    return UMI_STATUS_OK;
}

double umi_financial_workflow_instance_completion(
    const UmiFinancialWorkflowInstance *workflow)
{
    size_t i, completed = 0U;
    if (workflow == NULL || workflow->count == 0U) return 0.0;
    for (i = 0U; i < workflow->count; ++i)
        if (workflow->tasks[i].state == UMI_FINANCIAL_WORKFLOW_COMPLETED)
            completed += 1U;
    return ((double)completed / (double)workflow->count) * 100.0;
}

size_t umi_financial_workflow_instance_overdue_tasks(
    const UmiFinancialWorkflowInstance *workflow,
    uint64_t nowMilliseconds)
{
    size_t i, count = 0U;
    if (workflow == NULL) return 0U;
    for (i = 0U; i < workflow->count; ++i) {
        const UmiFinancialWorkflowTask *task = &workflow->tasks[i];
        if (task->state != UMI_FINANCIAL_WORKFLOW_COMPLETED &&
            task->dueMilliseconds > 0U &&
            nowMilliseconds > task->dueMilliseconds)
            count += 1U;
    }
    return count;
}
