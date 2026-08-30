/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/archive_plan.c
 *
 * PURPOSE:
 *   archive format, compression and deterministic-build policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/archive_plan.h"

void umi_dr_archive_plan_init(UmiDrArchivePlan*p){if(p!=NULL)*p=(UmiDrArchivePlan){0};}UmiStatus umi_dr_archive_plan_complete_step(UmiDrArchivePlan*p){if(p==NULL||p->completed_steps>=p->required_steps)return UMI_STATUS_INVALID_STATE;p->completed_steps++;return UMI_STATUS_OK;}bool umi_dr_archive_plan_ready(const UmiDrArchivePlan*p){return p!=NULL&&p->required_steps>0U&&p->completed_steps==p->required_steps;}
