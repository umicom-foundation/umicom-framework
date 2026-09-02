/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/workflow_execution.c
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
#include "umicom/integration/fabric/workflow_execution.h"
#include <string.h>
#include <limits.h>


/*
 * Initialise fabric workflow execution from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_workflow_execution_init(UmiFabricWorkflowExecution *execution,const char *workflow_id){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(execution==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(execution,0,sizeof(*execution));s=umi_fabric_copy_text(execution->workflow_id,sizeof(execution->workflow_id),workflow_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;execution->state=UMI_FABRIC_WORKFLOW_PENDING;return UMI_STATUS_OK;}
/*
 * Provide the fabric workflow execution transition operation used by this module and its
 * client applications.
 */
UmiStatus umi_fabric_workflow_execution_transition(UmiFabricWorkflowExecution *e,UmiFabricWorkflowState n){bool ok=false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e->state==UMI_FABRIC_WORKFLOW_PENDING&&n==UMI_FABRIC_WORKFLOW_RUNNING)ok=true;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(e->state==UMI_FABRIC_WORKFLOW_RUNNING&&(n==UMI_FABRIC_WORKFLOW_SUCCEEDED||n==UMI_FABRIC_WORKFLOW_FAILED))ok=true;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(e->state==UMI_FABRIC_WORKFLOW_FAILED&&n==UMI_FABRIC_WORKFLOW_COMPENSATING)ok=true;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(e->state==UMI_FABRIC_WORKFLOW_COMPENSATING&&n==UMI_FABRIC_WORKFLOW_COMPENSATED)ok=true;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!ok)return UMI_STATUS_INVALID_STATE;e->state=n;return UMI_STATUS_OK;}
