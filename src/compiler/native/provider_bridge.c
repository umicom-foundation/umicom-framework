/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/provider_bridge.c
 *
 * PURPOSE:
 *   Plan bootstrap provider fallback between Umicc, Clang and GCC without replacing existing provider implementations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/provider_bridge.h"
#include <string.h>
static void add(UmiNativeProviderPlan *p,UmiNativeProviderKind k,bool ok){if(ok&&p->count<3U){p->order[p->count++]=k;if(p->selected==0)p->selected=k;}}
UmiStatus umi_nc_provider_bridge_plan(const UmiNativeProviderAvailability *a,bool prefer,bool ready,UmiNativeProviderPlan *p){if(a==NULL||p==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(p,0,sizeof(*p));if(prefer&&ready)add(p,UMI_NC_PROVIDER_UMICC,a->umicc);add(p,UMI_NC_PROVIDER_CLANG,a->clang);add(p,UMI_NC_PROVIDER_GCC,a->gcc);if((!prefer||!ready)&&a->umicc)add(p,UMI_NC_PROVIDER_UMICC,true);return p->count==0U?UMI_STATUS_UNAVAILABLE:UMI_STATUS_OK;}
const char *umi_nc_provider_kind_name(UmiNativeProviderKind k){switch(k){case UMI_NC_PROVIDER_UMICC:return "umicc";case UMI_NC_PROVIDER_CLANG:return "clang";case UMI_NC_PROVIDER_GCC:return "gcc";default:return "unknown";}}
