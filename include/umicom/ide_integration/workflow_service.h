/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/workflow_service.h
 *
 * PURPOSE:
 *   Own current workflow policy/readiness evidence over canonical IDE bindings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_WORKFLOW_SERVICE_H
#define UMICOM_IDE_INTEGRATION_WORKFLOW_SERVICE_H

#include "umicom/ide_integration/workflow_evaluator.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ide workflow service data shared with callers of this public contract.
 */
typedef struct UmiIdeWorkflowService {
    UmiIdeIntegrationBindings *bindings;
    char workspace_root[UMI_IDE_INTEGRATION_PATH_CAPACITY];
    UmiIdeWorkflowPolicy policy;
    UmiIdeActiveContext context;
    UmiIdeWorkflowReport report;
    uint64_t revision;
} UmiIdeWorkflowService;

/**
 * Initialise ide workflow service from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ide_workflow_service_init(
    UmiIdeWorkflowService *service,
    UmiIdeIntegrationBindings *bindings,
    const char *workspace_root);

/**
 * Provide the ide workflow service set policy operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_workflow_service_set_policy(
    UmiIdeWorkflowService *service,
    const UmiIdeWorkflowPolicy *policy);

/**
 * Provide the ide workflow service refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_workflow_service_refresh(
    UmiIdeWorkflowService *service);

/**
 * Provide the ide workflow service ready operation used by this module and its client
 * applications.
 */
int umi_ide_workflow_service_ready(
    const UmiIdeWorkflowService *service);

#ifdef __cplusplus
}
#endif
#endif
