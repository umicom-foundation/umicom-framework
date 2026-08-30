/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/compatibility.h
 *
 * PURPOSE:
 *   Evaluate Framework ABI and semantic-version compatibility before plug-in
 *   code is loaded.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_COMPATIBILITY_H
#define UMICOM_PLUGIN_COMPATIBILITY_H

#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include "umicom/plugin/manifest.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPluginCompatibility { int compatible; char reason[256]; } UmiPluginCompatibility;
UmiStatus umi_plugin_compatibility_check(const UmiPluginManifest *manifest,
                                         uint32_t framework_abi,
                                         UmiVersion framework_version,
                                         UmiPluginCompatibility *out_result);
#ifdef __cplusplus
}
#endif

#endif
