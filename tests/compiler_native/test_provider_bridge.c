/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_provider_bridge.c
 *
 * PURPOSE:
 *   Regression coverage for plan bootstrap provider fallback between umicc, clang and gcc without replacing existing provider implementations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/provider_bridge.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeProviderAvailability a={true,true,true};UmiNativeProviderPlan p;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_nc_provider_bridge_plan(&a,true,true,&p)!=UMI_STATUS_OK)return 1;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p.selected!=UMI_NC_PROVIDER_UMICC||p.count!=3U)return 2;return 0;}
