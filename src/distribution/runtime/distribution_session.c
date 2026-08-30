/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/distribution_session.c
 *
 * PURPOSE:
 *   top-level distribution planning session aggregating runtime/package/update readiness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/distribution_session.h"

void umi_dr_distribution_session_init(UmiDrDistributionSession*s,const char*id){if(s!=NULL){*s=(UmiDrDistributionSession){0};s->state=UMI_DR_STATE_PLANNED;if(id!=NULL)(void)umi_dr_copy_text(s->id,sizeof(s->id),id);}}bool umi_dr_distribution_session_ready(const UmiDrDistributionSession*s){return s!=NULL&&s->id[0]!='\0'&&s->runtime_ready&&s->package_ready&&s->release_gate_passed&&s->deployment_ready;}UmiStatus umi_dr_distribution_session_commit(UmiDrDistributionSession*s){if(!umi_dr_distribution_session_ready(s))return UMI_STATUS_INVALID_STATE;s->state=UMI_DR_STATE_COMMITTED;s->revision++;return UMI_STATUS_OK;}
