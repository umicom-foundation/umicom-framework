/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/workflow_service.c
 *
 * PURPOSE:
 *   Implement refreshable workflow readiness over current Framework state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/workflow_service.h"

#include <string.h>
#include <stdlib.h>

/*
 * Initialise ide workflow service from caller-provided values so later operations receive
 * a known state.
 */
/* Workspace identity now supports an explicit closed state without inventing a path. The original initializer is retained for review and its public non-empty contract is preserved. The previous implementation is retained for engineering review. */
#if 0
UmiStatus umi_ide_workflow_service_init(
    UmiIdeWorkflowService *service,
    UmiIdeIntegrationBindings *bindings,
    const char *workspace_root)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || bindings == NULL ||
        workspace_root == NULL || workspace_root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(workspace_root);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= sizeof(service->workspace_root)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memset(service, 0, sizeof(*service));
    service->bindings = bindings;
    (void)memcpy(service->workspace_root, workspace_root, length + 1U);
    umi_ide_workflow_policy_init(&service->policy);
    service->revision = 1U;
    return umi_ide_workflow_service_refresh(service);
}
#endif
static UmiStatus WorkflowServiceInitialize(
    UmiIdeWorkflowService *service,
    UmiIdeIntegrationBindings *bindings,
    const char *workspace_root)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || bindings == NULL ||
        workspace_root == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(workspace_root);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= sizeof(service->workspace_root)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memset(service, 0, sizeof(*service));
    service->bindings = bindings;
    (void)memcpy(service->workspace_root, workspace_root, length + 1U);
    umi_ide_workflow_policy_init(&service->policy);
    service->revision = 1U;
    return umi_ide_workflow_service_refresh(service);
}
/* Closed workspaces have no filesystem authority. The explicit constructor
 * keeps the existing non-empty initializer contract intact for older callers. */
UmiStatus umi_ide_workflow_service_init_closed(
    UmiIdeWorkflowService *service, UmiIdeIntegrationBindings *bindings)
{
    return WorkflowServiceInitialize(service, bindings, "");
}

UmiStatus umi_ide_workflow_service_init(UmiIdeWorkflowService *service,
    UmiIdeIntegrationBindings *bindings, const char *workspace_root)
{
    if (workspace_root == NULL || workspace_root[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    return WorkflowServiceInitialize(service, bindings, workspace_root);
}

/* Evaluate a copied candidate first: a failed refresh must not leave the root
 * claiming one project while readiness still describes another project. */
UmiStatus umi_ide_workflow_service_set_workspace(
    UmiIdeWorkflowService *service, const char *workspace_root)
{
    if (service == NULL || service->bindings == NULL || workspace_root == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = strlen(workspace_root);
    if (length >= sizeof(service->workspace_root)) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiIdeWorkflowService *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *service;
    memcpy(candidate->workspace_root, workspace_root, length + 1U);
    UmiStatus status = umi_ide_workflow_service_refresh(candidate);
    if (status == UMI_STATUS_OK) *service = *candidate;
    free(candidate);
    return status;
}


/*
 * Provide the ide workflow service set policy operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_workflow_service_set_policy(
    UmiIdeWorkflowService *service,
    const UmiIdeWorkflowPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || policy == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    service->policy = *policy;
    service->revision += 1U;
    return umi_ide_workflow_service_refresh(service);
}

/*
 * Provide the ide workflow service refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_workflow_service_refresh(
    UmiIdeWorkflowService *service)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || service->bindings == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ide_active_context_snapshot(
        service->bindings,
        service->workspace_root,
        &service->context);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ide_workflow_evaluate(
        service->bindings,
        &service->context,
        &service->policy,
        &service->report);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) service->revision += 1U;
    return status;
}

/*
 * Provide the ide workflow service ready operation used by this module and its client
 * applications.
 */
int umi_ide_workflow_service_ready(
    const UmiIdeWorkflowService *service)
{
    return service != NULL && service->report.ready;
}
