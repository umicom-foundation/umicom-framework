/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/manifest.h
 *
 * PURPOSE:
 *   Describe the installed Umicom Framework SDK version, ABI and component inventory.
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
#ifndef UMICOM_SDK_MANIFEST_H
#define UMICOM_SDK_MANIFEST_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include "umicom/sdk/component.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk manifest data shared with callers of this public contract.
 */
typedef struct UmiSdkManifest { UmiVersion framework_version; uint32_t framework_abi; const UmiSdkComponent *components; size_t component_count; } UmiSdkManifest;
/**
 * Provide the sdk manifest current operation used by this module and its client
 * applications.
 */
UmiSdkManifest umi_sdk_manifest_current(const UmiSdkComponent *components,size_t count);
/**
 * Check that sdk manifest satisfies its contract before another service relies on it.
 */
UmiStatus umi_sdk_manifest_validate(const UmiSdkManifest *manifest);
#ifdef __cplusplus
}
#endif
#endif
