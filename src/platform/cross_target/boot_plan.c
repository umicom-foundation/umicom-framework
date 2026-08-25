/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/boot_plan.c
 *
 * PURPOSE:
 *   Produce phase-ordered boot service plans while preserving stable declaration order within each phase.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/boot_plan.h"

#include <string.h>
void umi_ct_boot_plan_init(UmiCtBootPlan*p){if(p!=NULL)memset(p,0,sizeof(*p));}
UmiStatus umi_ct_boot_plan_add(UmiCtBootPlan*p,const UmiCtBootService*s){if(p==NULL||umi_ct_boot_service_validate(s)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;if(p->count>=UMI_CT_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;p->services[p->count++]=*s;return UMI_STATUS_OK;}
UmiStatus umi_ct_boot_plan_sort(UmiCtBootPlan*p){size_t i,j;if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=1U;i<p->count;++i){UmiCtBootService x=p->services[i];j=i;while(j>0U&&(unsigned)p->services[j-1U].phase>(unsigned)x.phase){p->services[j]=p->services[j-1U];--j;}p->services[j]=x;}return UMI_STATUS_OK;}
