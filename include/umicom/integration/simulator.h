/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/simulator.h
 *
 * PURPOSE:
 *   Publish the public simulator contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_SIMULATOR_H
#define UMICOM_INTEGRATION_SIMULATOR_H
#include "umicom/integration/workflow.h"
#include "umicom/integration/execution_policy.h"
/**
 * Represent the integration trace entry data shared with callers of this public contract.
 */
typedef struct UmiIntegrationTraceEntry {
    size_t sequence;
    char node_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    UmiIntegrationDesignerNodeKind kind;
    UmiIntegrationDesignerRunStatus status;
    char detail[UMI_INTEGRATION_DESIGNER_TEXT_CAPACITY];
} UmiIntegrationTraceEntry;
/**
 * Represent the integration simulation data shared with callers of this public contract.
 */
typedef struct UmiIntegrationSimulation {
    char run_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char workflow_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    UmiIntegrationDesignerRunStatus status;
    UmiIntegrationTraceEntry trace[UMI_INTEGRATION_DESIGNER_MAX_TRACE];
    size_t trace_count;
    unsigned attempted_requests;
} UmiIntegrationSimulation;
/**
 * Provide the integration simulate operation used by this module and its client
 * applications.
 */
UmiStatus umi_integration_simulate(const UmiIntegrationWorkflow *workflow,const UmiIntegrationExecutionPolicy *policy,const char *run_id,UmiIntegrationSimulation *out_simulation);
#endif
