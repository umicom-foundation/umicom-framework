/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/runtime_compatibility.c
 *
 * PURPOSE:
 *   runtime requirement matching with missing-capability and version evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/runtime_compatibility.h"

UmiStatus umi_dr_runtime_compatibility_evaluate(const UmiDrRuntimeProfile*r,const UmiDrRuntimeProbe*a,UmiDrRuntimeCompatibility*out){if(r==NULL||a==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;out->platform_match=r->platform==a->platform;out->architecture_match=r->architecture==a->architecture;out->version_match=umi_dr_version_at_least(a->version,r->minimum_version);out->missing_capabilities=r->required_capabilities & ~a->capabilities;return UMI_STATUS_OK;}bool umi_dr_runtime_compatibility_ok(const UmiDrRuntimeCompatibility*r){return r!=NULL&&r->platform_match&&r->architecture_match&&r->version_match&&r->missing_capabilities==0U;}
