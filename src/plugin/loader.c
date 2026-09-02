/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/loader.c
 *
 * PURPOSE:
 *   Validate compatibility before invoking the existing portable dynamic-library
 *   boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/loader.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>
/*
 * Read plugin loader into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_plugin_loader_load(const UmiPluginManifest *manifest, UmiLoadedPlugin *out_plugin, char *out_reason, size_t reason_capacity)
{
    UmiPluginCompatibility compatibility; UmiVersion framework = {UMICOM_FRAMEWORK_VERSION_MAJOR, UMICOM_FRAMEWORK_VERSION_MINOR, UMICOM_FRAMEWORK_VERSION_PATCH}; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || out_plugin == NULL || out_reason == NULL || reason_capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_plugin, 0, sizeof(*out_plugin));
    status = umi_plugin_compatibility_check(manifest, UMICOM_FRAMEWORK_ABI_VERSION, framework, &compatibility);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) { (void)snprintf(out_reason, reason_capacity, "%s", compatibility.reason); return status; }
    status = umi_plugin_load(manifest->library_path, &out_plugin->library);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) { (void)snprintf(out_reason, reason_capacity, "dynamic library load failed: %s", umi_status_text(status)); return status; }
    out_plugin->manifest = *manifest; out_plugin->descriptor = umi_plugin_descriptor(out_plugin->library); (void)snprintf(out_reason, reason_capacity, "loaded"); return UMI_STATUS_OK;
}
/*
 * Provide the plugin loader unload operation used by this module and its client
 * applications.
 */
void umi_plugin_loader_unload(UmiLoadedPlugin *plugin)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (plugin == NULL) return; umi_plugin_unload(plugin->library); (void)memset(plugin, 0, sizeof(*plugin)); }
