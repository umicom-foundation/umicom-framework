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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/boot_plan.h"

#include <string.h>
/*
 * Initialise ct boot plan from caller-provided values so later operations receive a known
 * state.
 */
void umi_ct_boot_plan_init(UmiCtBootPlan*p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL)memset(p,0,sizeof(*p));}
/* Add ct boot plan only after its inputs and available capacity have been checked. */
UmiStatus umi_ct_boot_plan_add(UmiCtBootPlan*p,const UmiCtBootService*s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||umi_ct_boot_service_validate(s)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->count>=UMI_CT_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;p->services[p->count++]=*s;return UMI_STATUS_OK;}
/* Provide the ct boot plan sort operation used by this module and its client applications. */
UmiStatus umi_ct_boot_plan_sort(UmiCtBootPlan*p){size_t i,j;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=1U;i<p->count;++i){UmiCtBootService x=p->services[i];j=i;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(j>0U&&(unsigned)p->services[j-1U].phase>(unsigned)x.phase){p->services[j]=p->services[j-1U];--j;}p->services[j]=x;}return UMI_STATUS_OK;}
