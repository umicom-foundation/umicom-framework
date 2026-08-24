/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/workflow_service.h
 *
 * PURPOSE:
 *   Own current workflow policy/readiness evidence over canonical IDE bindings.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_WORKFLOW_SERVICE_H
#define UMICOM_IDE_INTEGRATION_WORKFLOW_SERVICE_H

#include "umicom/ide_integration/workflow_evaluator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiIdeWorkflowService {
    UmiIdeIntegrationBindings *bindings;
    char workspace_root[UMI_IDE_INTEGRATION_PATH_CAPACITY];
    UmiIdeWorkflowPolicy policy;
    UmiIdeActiveContext context;
    UmiIdeWorkflowReport report;
    uint64_t revision;
} UmiIdeWorkflowService;

UmiStatus umi_ide_workflow_service_init(
    UmiIdeWorkflowService *service,
    UmiIdeIntegrationBindings *bindings,
    const char *workspace_root);

UmiStatus umi_ide_workflow_service_set_policy(
    UmiIdeWorkflowService *service,
    const UmiIdeWorkflowPolicy *policy);

UmiStatus umi_ide_workflow_service_refresh(
    UmiIdeWorkflowService *service);

int umi_ide_workflow_service_ready(
    const UmiIdeWorkflowService *service);

#ifdef __cplusplus
}
#endif
#endif
