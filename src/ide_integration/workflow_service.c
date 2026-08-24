/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/workflow_service.c
 *
 * PURPOSE:
 *   Implement refreshable workflow readiness over current Framework state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/workflow_service.h"

#include <string.h>

UmiStatus umi_ide_workflow_service_init(
    UmiIdeWorkflowService *service,
    UmiIdeIntegrationBindings *bindings,
    const char *workspace_root)
{
    size_t length;

    if (service == NULL || bindings == NULL ||
        workspace_root == NULL || workspace_root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(workspace_root);
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

UmiStatus umi_ide_workflow_service_set_policy(
    UmiIdeWorkflowService *service,
    const UmiIdeWorkflowPolicy *policy)
{
    if (service == NULL || policy == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    service->policy = *policy;
    service->revision += 1U;
    return umi_ide_workflow_service_refresh(service);
}

UmiStatus umi_ide_workflow_service_refresh(
    UmiIdeWorkflowService *service)
{
    UmiStatus status;

    if (service == NULL || service->bindings == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ide_active_context_snapshot(
        service->bindings,
        service->workspace_root,
        &service->context);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ide_workflow_evaluate(
        service->bindings,
        &service->context,
        &service->policy,
        &service->report);
    if (status == UMI_STATUS_OK) service->revision += 1U;
    return status;
}

int umi_ide_workflow_service_ready(
    const UmiIdeWorkflowService *service)
{
    return service != NULL && service->report.ready;
}
