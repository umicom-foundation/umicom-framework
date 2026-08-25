/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_self_host_gate.c
 *
 * PURPOSE:
 *   Regression coverage for evaluate evidence for moving umicc from experimental bootstrap use toward controlled self-hosting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/self_host_gate.h"
int main(void){UmiNativeSelfHostEvidence e={true,true,true,true,true,true,true,true,true,true,100U,100U,0U};UmiNativeSelfHostAssessment a=umi_nc_self_host_assess(&e);if(a.stage!=UMI_NC_SELFHOST_SELFHOST||!a.ready_for_default||a.coverage!=1.0)return 1;return 0;}
