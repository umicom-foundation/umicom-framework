/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/os_service_dependency.c
 *
 * PURPOSE:
 *   Represent service dependency edges and distinguish required versus optional boot dependencies.
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

#include "umicom/platform/cross_target/os_service_dependency.h"

#include <string.h>
UmiStatus umi_ct_os_service_dependency_init(UmiCtOsServiceDependency*d,const char*s,const char*p,bool r){if(d==NULL||!umi_ct_id_valid(s)||!umi_ct_id_valid(p))return UMI_STATUS_INVALID_ARGUMENT;if(umi_ct_copy(d->service_id,sizeof(d->service_id),s)!=UMI_STATUS_OK||umi_ct_copy(d->dependency_id,sizeof(d->dependency_id),p)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;d->required=r;return strcmp(s,p)==0?UMI_STATUS_INVALID_STATE:UMI_STATUS_OK;}
bool umi_ct_os_service_dependency_self_cycle(const UmiCtOsServiceDependency*d){return d!=NULL&&strcmp(d->service_id,d->dependency_id)==0;}
