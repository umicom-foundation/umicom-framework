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

#include "umicom/platform/cross_target/cross_target_service.h"
#include "umicom/platform/cross_target/target_triple.h"

#include <string.h>
void umi_ct_cross_target_service_init(UmiCtCrossTargetService*s){if(s!=NULL){memset(s,0,sizeof(*s));umi_ct_target_catalogue_init(&s->targets);}}
UmiStatus umi_ct_cross_target_service_register(UmiCtCrossTargetService*s,const UmiCtTargetProfile*p){UmiStatus st;if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;st=umi_ct_target_catalogue_add(&s->targets,p);if(st==UMI_STATUS_OK)s->revision++;return st;}
UmiStatus umi_ct_cross_target_service_activate(UmiCtCrossTargetService*s,const char*id,const UmiCtCrossTargetSnapshot*x){const UmiCtTargetProfile*p;if(s==NULL||id==NULL||x==NULL)return UMI_STATUS_INVALID_ARGUMENT;p=umi_ct_target_catalogue_find(&s->targets,id);if(p==NULL)return UMI_STATUS_NOT_FOUND;if(!umi_ct_target_compatible(&p->target,&x->target))return UMI_STATUS_INVALID_STATE;if(umi_ct_cross_target_snapshot_validate(x)!=UMI_STATUS_OK)return UMI_STATUS_UNAVAILABLE;s->active=*x;s->active_set=true;s->revision++;return UMI_STATUS_OK;}
const UmiCtCrossTargetSnapshot *umi_ct_cross_target_service_active(const UmiCtCrossTargetService*s){return s!=NULL&&s->active_set?&s->active:NULL;}
