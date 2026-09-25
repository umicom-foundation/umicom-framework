/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/workflow_instance.h
 *
 * PURPOSE:
 *   Track bounded financial workflow tasks, approvals, completion, escalation
 *   and rollback evidence independently of any application frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_WORKFLOW_INSTANCE_H
#define UMICOM_FINANCIAL_PLATFORM_WORKFLOW_INSTANCE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FINANCIAL_WORKFLOW_TASK_CAPACITY 64U

typedef enum UmiFinancialWorkflowTaskState {
    UMI_FINANCIAL_WORKFLOW_PENDING = 0,
    UMI_FINANCIAL_WORKFLOW_APPROVAL_PENDING = 1,
    UMI_FINANCIAL_WORKFLOW_COMPLETED = 2,
    UMI_FINANCIAL_WORKFLOW_REJECTED = 3,
    UMI_FINANCIAL_WORKFLOW_ROLLED_BACK = 4
} UmiFinancialWorkflowTaskState;

typedef struct UmiFinancialWorkflowTask {
    char taskId[64];
    char role[48];
    uint64_t dueMilliseconds;
    UmiFinancialWorkflowTaskState state;
    int approvalRequired;
    int approved;
} UmiFinancialWorkflowTask;

typedef struct UmiFinancialWorkflowInstance {
    char workflowId[64];
    char subjectId[64];
    UmiFinancialWorkflowTask tasks[UMI_FINANCIAL_WORKFLOW_TASK_CAPACITY];
    size_t count;
    uint64_t revision;
    int rollbackAvailable;
} UmiFinancialWorkflowInstance;

void umi_financial_workflow_instance_init(
    UmiFinancialWorkflowInstance *workflow,
    const char *workflowId,
    const char *subjectId);

UmiStatus umi_financial_workflow_instance_add_task(
    UmiFinancialWorkflowInstance *workflow,
    const UmiFinancialWorkflowTask *task);

UmiStatus umi_financial_workflow_instance_approve(
    UmiFinancialWorkflowInstance *workflow,
    const char *taskId);

UmiStatus umi_financial_workflow_instance_complete(
    UmiFinancialWorkflowInstance *workflow,
    const char *taskId);

UmiStatus umi_financial_workflow_instance_rollback(
    UmiFinancialWorkflowInstance *workflow,
    const char *taskId);

double umi_financial_workflow_instance_completion(
    const UmiFinancialWorkflowInstance *workflow);

size_t umi_financial_workflow_instance_overdue_tasks(
    const UmiFinancialWorkflowInstance *workflow,
    uint64_t nowMilliseconds);

#ifdef __cplusplus
}
#endif
#endif
