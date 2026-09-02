/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/validation_plan.c
 *
 * PURPOSE:
 *   Collect validation gates and determine whether required evidence is complete.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/validation_plan.h"
/* Append a structurally valid gate to bounded validation storage. */
UmiStatus umi_helix_validation_plan_add(UmiHelixValidationPlan *p,const UmiHelixValidationGate *g){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||g==NULL||umi_helix_validation_gate_validate(g)!=UMI_STATUS_OK){ return UMI_STATUS_INVALID_ARGUMENT; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->count>=UMI_HELIX_MAX_ITEMS){ return UMI_STATUS_CAPACITY_EXCEEDED; } p->gates[p->count]=*g; p->completed[p->count]=false; p->passed[p->count]=false; p->count++; return UMI_STATUS_OK; }
/* Store validation evidence without altering the gate policy. */
UmiStatus umi_helix_validation_plan_record(UmiHelixValidationPlan *p,size_t i,bool passed){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||i>=p->count){ return UMI_STATUS_INVALID_ARGUMENT; } p->completed[i]=true; p->passed[i]=passed; return UMI_STATUS_OK; }
/* Require every blocking gate to be both observed and successful. */
bool umi_helix_validation_plan_ready(const UmiHelixValidationPlan *p){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL){ return false; } /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<p->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->gates[i].blocking&&(!p->completed[i]||!p->passed[i])){ return false; } return true; }
