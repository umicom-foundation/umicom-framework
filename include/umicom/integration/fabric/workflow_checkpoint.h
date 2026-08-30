/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/workflow_checkpoint.h
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
#ifndef UMICOM_INTEGRATION_FABRIC_WORKFLOW_CHECKPOINT_H
#define UMICOM_INTEGRATION_FABRIC_WORKFLOW_CHECKPOINT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/workflow_plan.h"
#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricWorkflowCheckpoint { char workflow_id[UMI_FABRIC_ID_CAPACITY]; size_t next_step; uint64_t plan_fingerprint; uint64_t updated_ms; } UmiFabricWorkflowCheckpoint;
UmiStatus umi_fabric_workflow_checkpoint_init(UmiFabricWorkflowCheckpoint *checkpoint,const char *workflow_id,const UmiFabricWorkflowPlan *plan,uint64_t now_ms);
UmiStatus umi_fabric_workflow_checkpoint_advance(UmiFabricWorkflowCheckpoint *checkpoint,const UmiFabricWorkflowPlan *plan,uint64_t now_ms);

#ifdef __cplusplus
}
#endif
#endif
