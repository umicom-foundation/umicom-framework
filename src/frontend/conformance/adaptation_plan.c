/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/adaptation_plan.c
 *
 * PURPOSE:
 *   reviewable frontend adaptation operations for unsupported or constrained capabilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/adaptation_plan.h"

void umi_fc_adaptation_plan_build(uint64_t missing,uint64_t emulatable,uint64_t hideable,UmiFcAdaptationPlan *out_plan){if(out_plan==NULL)return;out_plan->missing=missing;out_plan->emulated=missing&emulatable;out_plan->hidden=(missing&~out_plan->emulated)&hideable;out_plan->operations=(size_t)__builtin_popcountll(out_plan->emulated|out_plan->hidden);out_plan->review_required=missing!=0U;}
UmiFcOutcome umi_fc_adaptation_plan_outcome(const UmiFcAdaptationPlan *plan){uint64_t unresolved;if(plan==NULL)return UMI_FC_FAIL;unresolved=plan->missing&~(plan->emulated|plan->hidden);if(unresolved!=0U)return UMI_FC_FAIL;if(plan->missing!=0U)return UMI_FC_DEGRADED;return UMI_FC_PASS;}
