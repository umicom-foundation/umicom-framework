/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/installer_plan.c
 *
 * PURPOSE:
 *   installer operations with privilege and reboot evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/installer_plan.h"

void umi_dr_installer_plan_init(UmiDrInstallerPlan*p){if(p!=NULL)*p=(UmiDrInstallerPlan){0};}UmiStatus umi_dr_installer_plan_complete_step(UmiDrInstallerPlan*p){if(p==NULL||p->completed_steps>=p->required_steps)return UMI_STATUS_INVALID_STATE;p->completed_steps++;return UMI_STATUS_OK;}bool umi_dr_installer_plan_ready(const UmiDrInstallerPlan*p){return p!=NULL&&p->required_steps>0U&&p->completed_steps==p->required_steps;}
