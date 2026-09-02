/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/compatibility.h
 *
 * PURPOSE:
 *   Compare an SDK manifest against consumer version and ABI requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * SDK APIs verify that an application can consume an installed Umicom Framework package without depending on private source-tree details.
 */
#ifndef UMICOM_SDK_COMPATIBILITY_H
#define UMICOM_SDK_COMPATIBILITY_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include "umicom/sdk/manifest.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk requirement data shared with callers of this public contract.
 */
typedef struct UmiSdkRequirement { UmiVersion minimum_version; uint32_t required_abi; } UmiSdkRequirement;
/**
 * Provide the sdk compatible operation used by this module and its client applications.
 */
int umi_sdk_compatible(const UmiSdkRequirement *requirement,const UmiSdkManifest *manifest);
#ifdef __cplusplus
}
#endif
#endif
