/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/compatibility.c
 *
 * PURPOSE:
 *   Implement ABI equality and conservative major-version compatibility checks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/compatibility.h"

#include <stddef.h>
#include <stdio.h>
UmiStatus umi_plugin_compatibility_check(const UmiPluginManifest *manifest, uint32_t framework_abi, UmiVersion framework_version, UmiPluginCompatibility *out_result)
{
    if (manifest == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    out_result->compatible = 0;
    if (manifest->required_abi != framework_abi) { (void)snprintf(out_result->reason, sizeof(out_result->reason), "ABI %u required, host provides %u", manifest->required_abi, framework_abi); return UMI_STATUS_INVALID_STATE; }
    if (manifest->version.major > framework_version.major + 1U) { (void)snprintf(out_result->reason, sizeof(out_result->reason), "plug-in major version is not supported"); return UMI_STATUS_INVALID_STATE; }
    out_result->compatible = 1; (void)snprintf(out_result->reason, sizeof(out_result->reason), "compatible"); return UMI_STATUS_OK;
}
