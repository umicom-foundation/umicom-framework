/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/portable_plan.c
 *
 * PURPOSE:
 *   self-contained portable bundle policy and relocatability validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/portable_plan.h"

/*
 * Initialise dr portable plan from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_portable_plan_init(UmiDrPortablePlan*p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL)*p=(UmiDrPortablePlan){0};}UmiStatus umi_dr_portable_plan_complete_step(UmiDrPortablePlan*p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||p->completed_steps>=p->required_steps)return UMI_STATUS_INVALID_STATE;p->completed_steps++;return UMI_STATUS_OK;}bool umi_dr_portable_plan_ready(const UmiDrPortablePlan*p){return p!=NULL&&p->required_steps>0U&&p->completed_steps==p->required_steps;}
