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
typedef struct UmiSdkManifest { UmiVersion framework_version; uint32_t framework_abi; const UmiSdkComponent *components; size_t component_count; } UmiSdkManifest;
UmiSdkManifest umi_sdk_manifest_current(const UmiSdkComponent *components,size_t count);
UmiStatus umi_sdk_manifest_validate(const UmiSdkManifest *manifest);
#ifdef __cplusplus
}
#endif
#endif
