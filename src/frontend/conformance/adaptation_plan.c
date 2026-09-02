/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/adaptation_plan.c
 *
 * PURPOSE:
 *   reviewable frontend adaptation operations for unsupported or constrained capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/adaptation_plan.h"

/*
 * Provide the fc adaptation plan build operation used by this module and its client
 * applications.
 */
void umi_fc_adaptation_plan_build(uint64_t missing,uint64_t emulatable,uint64_t hideable,UmiFcAdaptationPlan *out_plan){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_plan==NULL)return;out_plan->missing=missing;out_plan->emulated=missing&emulatable;out_plan->hidden=(missing&~out_plan->emulated)&hideable;out_plan->operations=(size_t)__builtin_popcountll(out_plan->emulated|out_plan->hidden);out_plan->review_required=missing!=0U;}
/*
 * Provide the fc adaptation plan outcome operation used by this module and its client
 * applications.
 */
UmiFcOutcome umi_fc_adaptation_plan_outcome(const UmiFcAdaptationPlan *plan){uint64_t unresolved;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(plan==NULL)return UMI_FC_FAIL;unresolved=plan->missing&~(plan->emulated|plan->hidden);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(unresolved!=0U)return UMI_FC_FAIL;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(plan->missing!=0U)return UMI_FC_DEGRADED;return UMI_FC_PASS;}
