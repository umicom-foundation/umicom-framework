/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/compensation_policy.c
 *
 * PURPOSE:
 *   Build reverse compensation order from completed saga steps while skipping non-compensatable actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/compensation_policy.h"
#include <string.h>
#include <limits.h>


/*
 * Provide the fabric compensation policy default operation used by this module and its
 * client applications.
 */
void umi_fabric_compensation_policy_default(UmiFabricCompensationPolicy *policy){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy!=NULL){policy->stop_on_failure=true;policy->maximum_compensations=UMI_FABRIC_MAX_STEPS;}}
/*
 * Provide the fabric compensation order operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_compensation_order(const UmiFabricSagaPlan *plan,size_t completed_steps,char out_step_ids[UMI_FABRIC_MAX_STEPS][UMI_FABRIC_ID_CAPACITY],size_t *out_count){size_t i,n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(plan==NULL||out_step_ids==NULL||out_count==NULL||completed_steps>plan->count)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=completed_steps;i>0U;--i){const UmiFabricSagaStep *s=&plan->items[i-1U];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->compensation_required){(void)umi_fabric_copy_text(out_step_ids[n],UMI_FABRIC_ID_CAPACITY,s->step_id);n++;}}*out_count=n;return UMI_STATUS_OK;}
