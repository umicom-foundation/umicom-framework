/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_provider_bridge.c
 *
 * PURPOSE:
 *   Regression coverage for plan bootstrap provider fallback between umicc, clang and gcc without replacing existing provider implementations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/provider_bridge.h"
int main(void){UmiNativeProviderAvailability a={true,true,true};UmiNativeProviderPlan p;if(umi_nc_provider_bridge_plan(&a,true,true,&p)!=UMI_STATUS_OK)return 1;if(p.selected!=UMI_NC_PROVIDER_UMICC||p.count!=3U)return 2;return 0;}
