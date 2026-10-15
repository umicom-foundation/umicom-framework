/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/cross_target_service.c
 *
 * PURPOSE:
 *   Provide the aggregate Framework cross-target readiness service consumed by CLI, Studio, Desk and future Umicom OS tooling.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdint.h>
#include "umicom/platform/cross_target/cross_target_service.h"
#include "umicom/platform/cross_target/target_triple.h"

#include <string.h>
/*
 * Initialise ct cross target service from caller-provided values so later operations
 * receive a known state.
 */
void umi_ct_cross_target_service_init(UmiCtCrossTargetService*s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL){memset(s,0,sizeof(*s));umi_ct_target_catalogue_init(&s->targets);}}
/*
 * Add ct cross target service only after its inputs and available capacity have been
 * checked.
 */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_ct_cross_target_service_register(UmiCtCrossTargetService*s,const UmiCtTargetProfile*p){UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;st=umi_ct_target_catalogue_add(&s->targets,p);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)s->revision++;return st;}
#endif

UmiStatus umi_ct_cross_target_service_register(UmiCtCrossTargetService*s,const UmiCtTargetProfile*p){
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (s != NULL && s->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;st=umi_ct_target_catalogue_add(&s->targets,p);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)s->revision++;return st;}
/*
 * Provide the ct cross target service activate operation used by this module and its
 * client applications.
 */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_ct_cross_target_service_activate(UmiCtCrossTargetService*s,const char*id,const UmiCtCrossTargetSnapshot*x){const UmiCtTargetProfile*p;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||id==NULL||x==NULL)return UMI_STATUS_INVALID_ARGUMENT;p=umi_ct_target_catalogue_find(&s->targets,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_NOT_FOUND;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_ct_target_compatible(&p->target,&x->target))return UMI_STATUS_INVALID_STATE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_ct_cross_target_snapshot_validate(x)!=UMI_STATUS_OK)return UMI_STATUS_UNAVAILABLE;s->active=*x;s->active_set=true;s->revision++;return UMI_STATUS_OK;}
#endif

UmiStatus umi_ct_cross_target_service_activate(UmiCtCrossTargetService*s,const char*id,const UmiCtCrossTargetSnapshot*x){
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (s != NULL && s->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
const UmiCtTargetProfile*p;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||id==NULL||x==NULL)return UMI_STATUS_INVALID_ARGUMENT;p=umi_ct_target_catalogue_find(&s->targets,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_NOT_FOUND;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_ct_target_compatible(&p->target,&x->target))return UMI_STATUS_INVALID_STATE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_ct_cross_target_snapshot_validate(x)!=UMI_STATUS_OK)return UMI_STATUS_UNAVAILABLE;s->active=*x;s->active_set=true;s->revision++;return UMI_STATUS_OK;}
/*
 * Provide the ct cross target service active operation used by this module and its client
 * applications.
 */
const UmiCtCrossTargetSnapshot *umi_ct_cross_target_service_active(const UmiCtCrossTargetService*s){return s!=NULL&&s->active_set?&s->active:NULL;}
