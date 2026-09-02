/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/workflow_execution.h
 *
 * PURPOSE:
 *   Maintain explicit workflow execution state transitions without performing external side effects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_WORKFLOW_EXECUTION_H
#define UMICOM_INTEGRATION_FABRIC_WORKFLOW_EXECUTION_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * List the named fabric workflow state values accepted by this public contract.
 */
typedef enum UmiFabricWorkflowState { UMI_FABRIC_WORKFLOW_PENDING=1,UMI_FABRIC_WORKFLOW_RUNNING=2,UMI_FABRIC_WORKFLOW_SUCCEEDED=3,UMI_FABRIC_WORKFLOW_FAILED=4,UMI_FABRIC_WORKFLOW_COMPENSATING=5,UMI_FABRIC_WORKFLOW_COMPENSATED=6 } UmiFabricWorkflowState;
/**
 * Represent the fabric workflow execution data shared with callers of this public
 * contract.
 */
typedef struct UmiFabricWorkflowExecution { char workflow_id[UMI_FABRIC_ID_CAPACITY]; UmiFabricWorkflowState state; size_t completed_steps; size_t failed_step; } UmiFabricWorkflowExecution;
/**
 * Initialise fabric workflow execution from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_workflow_execution_init(UmiFabricWorkflowExecution *execution,const char *workflow_id);
/**
 * Provide the fabric workflow execution transition operation used by this module and its
 * client applications.
 */
UmiStatus umi_fabric_workflow_execution_transition(UmiFabricWorkflowExecution *execution,UmiFabricWorkflowState next_state);

#ifdef __cplusplus
}
#endif
#endif
