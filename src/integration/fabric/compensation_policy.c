/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/compensation_policy.c
 *
 * PURPOSE:
 *   Build reverse compensation order from completed saga steps while skipping non-compensatable actions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/compensation_policy.h"
#include <string.h>
#include <limits.h>


void umi_fabric_compensation_policy_default(UmiFabricCompensationPolicy *policy){if(policy!=NULL){policy->stop_on_failure=true;policy->maximum_compensations=UMI_FABRIC_MAX_STEPS;}}
UmiStatus umi_fabric_compensation_order(const UmiFabricSagaPlan *plan,size_t completed_steps,char out_step_ids[UMI_FABRIC_MAX_STEPS][UMI_FABRIC_ID_CAPACITY],size_t *out_count){size_t i,n=0U;if(plan==NULL||out_step_ids==NULL||out_count==NULL||completed_steps>plan->count)return UMI_STATUS_INVALID_ARGUMENT;for(i=completed_steps;i>0U;--i){const UmiFabricSagaStep *s=&plan->items[i-1U];if(s->compensation_required){(void)umi_fabric_copy_text(out_step_ids[n],UMI_FABRIC_ID_CAPACITY,s->step_id);n++;}}*out_count=n;return UMI_STATUS_OK;}
