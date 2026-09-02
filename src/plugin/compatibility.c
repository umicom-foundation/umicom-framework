/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/compatibility.c
 *
 * PURPOSE:
 *   Implement ABI equality and conservative major-version compatibility checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/compatibility.h"

#include <stddef.h>
#include <stdio.h>
/*
 * Provide the plugin compatibility check operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_compatibility_check(const UmiPluginManifest *manifest, uint32_t framework_abi, UmiVersion framework_version, UmiPluginCompatibility *out_result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    out_result->compatible = 0;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (manifest->required_abi != framework_abi) { (void)snprintf(out_result->reason, sizeof(out_result->reason), "ABI %u required, host provides %u", manifest->required_abi, framework_abi); return UMI_STATUS_INVALID_STATE; }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (manifest->version.major > framework_version.major + 1U) { (void)snprintf(out_result->reason, sizeof(out_result->reason), "plug-in major version is not supported"); return UMI_STATUS_INVALID_STATE; }
    out_result->compatible = 1; (void)snprintf(out_result->reason, sizeof(out_result->reason), "compatible"); return UMI_STATUS_OK;
}
