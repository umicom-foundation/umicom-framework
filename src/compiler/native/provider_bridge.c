/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/provider_bridge.c
 *
 * PURPOSE:
 *   Plan bootstrap provider fallback between Umicc, Clang and GCC without replacing existing provider implementations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/provider_bridge.h"
#include <string.h>
/* Provide the add operation used by this module and its client applications. */
static void add(UmiNativeProviderPlan *p,UmiNativeProviderKind k,bool ok){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(ok&&p->count<3U){p->order[p->count++]=k;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->selected==0)p->selected=k;}}
/*
 * Provide the nc provider bridge plan operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_provider_bridge_plan(const UmiNativeProviderAvailability *a,bool prefer,bool ready,UmiNativeProviderPlan *p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a==NULL||p==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(p,0,sizeof(*p));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(prefer&&ready)add(p,UMI_NC_PROVIDER_UMICC,a->umicc);add(p,UMI_NC_PROVIDER_CLANG,a->clang);add(p,UMI_NC_PROVIDER_GCC,a->gcc);/* Protect caller-owned memory by checking that required state is available before it is used. */ if((!prefer||!ready)&&a->umicc)add(p,UMI_NC_PROVIDER_UMICC,true);return p->count==0U?UMI_STATUS_UNAVAILABLE:UMI_STATUS_OK;}
/*
 * Provide the nc provider kind name operation used by this module and its client
 * applications.
 */
const char *umi_nc_provider_kind_name(UmiNativeProviderKind k){/* Select the behaviour associated with the requested command or state value. */ switch(k){case UMI_NC_PROVIDER_UMICC:return "umicc";case UMI_NC_PROVIDER_CLANG:return "clang";case UMI_NC_PROVIDER_GCC:return "gcc";default:return "unknown";}}
