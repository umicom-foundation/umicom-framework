/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/workflow_checkpoint.c
 *
 * PURPOSE:
 *   Record resumable workflow progress with a plan fingerprint continuity gate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/workflow_checkpoint.h"
#include <string.h>
#include <limits.h>


UmiStatus umi_fabric_workflow_checkpoint_init(UmiFabricWorkflowCheckpoint *checkpoint,const char *workflow_id,const UmiFabricWorkflowPlan *plan,uint64_t now_ms){UmiStatus s;if(checkpoint==NULL||plan==NULL||plan->count==0U)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(checkpoint,0,sizeof(*checkpoint));s=umi_fabric_copy_text(checkpoint->workflow_id,sizeof(checkpoint->workflow_id),workflow_id);if(s!=UMI_STATUS_OK)return s;checkpoint->plan_fingerprint=plan->fingerprint;checkpoint->updated_ms=now_ms;return UMI_STATUS_OK;}
UmiStatus umi_fabric_workflow_checkpoint_advance(UmiFabricWorkflowCheckpoint *checkpoint,const UmiFabricWorkflowPlan *plan,uint64_t now_ms){if(checkpoint==NULL||plan==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(checkpoint->plan_fingerprint!=plan->fingerprint)return UMI_STATUS_INVALID_STATE;if(checkpoint->next_step>=plan->count)return UMI_STATUS_INVALID_STATE;checkpoint->next_step++;checkpoint->updated_ms=now_ms;return UMI_STATUS_OK;}
