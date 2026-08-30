/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/package_plan.c
 *
 * PURPOSE:
 *   ordered package build stages and readiness checking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/package_plan.h"

void umi_dr_package_plan_init(UmiDrPackagePlan*p){if(p!=NULL)*p=(UmiDrPackagePlan){0};}UmiStatus umi_dr_package_plan_complete_step(UmiDrPackagePlan*p){if(p==NULL||p->completed_steps>=p->required_steps)return UMI_STATUS_INVALID_STATE;p->completed_steps++;return UMI_STATUS_OK;}bool umi_dr_package_plan_ready(const UmiDrPackagePlan*p){return p!=NULL&&p->required_steps>0U&&p->completed_steps==p->required_steps;}
