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

void umi_dr_portable_plan_init(UmiDrPortablePlan*p){if(p!=NULL)*p=(UmiDrPortablePlan){0};}UmiStatus umi_dr_portable_plan_complete_step(UmiDrPortablePlan*p){if(p==NULL||p->completed_steps>=p->required_steps)return UMI_STATUS_INVALID_STATE;p->completed_steps++;return UMI_STATUS_OK;}bool umi_dr_portable_plan_ready(const UmiDrPortablePlan*p){return p!=NULL&&p->required_steps>0U&&p->completed_steps==p->required_steps;}
