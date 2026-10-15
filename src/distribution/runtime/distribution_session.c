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
#include <stdint.h>
#include "umicom/distribution/runtime/distribution_session.h"

/*
 * Initialise dr distribution session from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_distribution_session_init(UmiDrDistributionSession*s,const char*id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL){*s=(UmiDrDistributionSession){0};s->state=UMI_DR_STATE_PLANNED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(id!=NULL)(void)umi_dr_copy_text(s->id,sizeof(s->id),id);}}bool umi_dr_distribution_session_ready(const UmiDrDistributionSession*s){return s!=NULL&&s->id[0]!='\0'&&s->runtime_ready&&s->package_ready&&s->release_gate_passed&&s->deployment_ready;}/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_dr_distribution_session_commit(UmiDrDistributionSession*s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_dr_distribution_session_ready(s))return UMI_STATUS_INVALID_STATE;s->state=UMI_DR_STATE_COMMITTED;s->revision++;return UMI_STATUS_OK;}
#endif

UmiStatus umi_dr_distribution_session_commit(UmiDrDistributionSession*s){
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (s != NULL && s->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_dr_distribution_session_ready(s))return UMI_STATUS_INVALID_STATE;s->state=UMI_DR_STATE_COMMITTED;s->revision++;return UMI_STATUS_OK;}
