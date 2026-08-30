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
UmiStatus umi_helix_validation_plan_add(UmiHelixValidationPlan *p,const UmiHelixValidationGate *g){ if(p==NULL||g==NULL||umi_helix_validation_gate_validate(g)!=UMI_STATUS_OK){ return UMI_STATUS_INVALID_ARGUMENT; } if(p->count>=UMI_HELIX_MAX_ITEMS){ return UMI_STATUS_CAPACITY_EXCEEDED; } p->gates[p->count]=*g; p->completed[p->count]=false; p->passed[p->count]=false; p->count++; return UMI_STATUS_OK; }
/* Store validation evidence without altering the gate policy. */
UmiStatus umi_helix_validation_plan_record(UmiHelixValidationPlan *p,size_t i,bool passed){ if(p==NULL||i>=p->count){ return UMI_STATUS_INVALID_ARGUMENT; } p->completed[i]=true; p->passed[i]=passed; return UMI_STATUS_OK; }
/* Require every blocking gate to be both observed and successful. */
bool umi_helix_validation_plan_ready(const UmiHelixValidationPlan *p){ if(p==NULL){ return false; } for(size_t i=0;i<p->count;i++) if(p->gates[i].blocking&&(!p->completed[i]||!p->passed[i])){ return false; } return true; }
