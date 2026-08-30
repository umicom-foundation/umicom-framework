/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/host.c
 *
 * PURPOSE:
 *   Construct the application plug-in host and enforce permission grants before
 *   registration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/host.h"

#include <stddef.h>
#include "umicom/plugin/permission.h"
#include <stdlib.h>
struct UmiPluginHost { UmiPluginRegistry *registry; UmiPluginContributionRegistry *contributions; UmiPermissionSet *grants; };
UmiStatus umi_plugin_host_create(UmiPluginHost **out_host)
{
    UmiPluginHost *host; UmiStatus status;
    if (out_host == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_host = NULL; host = (UmiPluginHost *)calloc(1U, sizeof(*host)); if (host == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_plugin_registry_create(&host->registry); if (status == UMI_STATUS_OK) status = umi_plugin_contribution_registry_create(&host->contributions); if (status == UMI_STATUS_OK) status = umi_permission_set_create(&host->grants);
    if (status != UMI_STATUS_OK) { umi_plugin_host_destroy(host); return status; }
    *out_host = host; return UMI_STATUS_OK;
}
void umi_plugin_host_destroy(UmiPluginHost *host)
{ if (host == NULL) return; umi_permission_set_destroy(host->grants); umi_plugin_contribution_registry_destroy(host->contributions); umi_plugin_registry_destroy(host->registry); free(host); }
UmiPluginRegistry *umi_plugin_host_registry(UmiPluginHost *host) { return host != NULL ? host->registry : NULL; }
UmiPluginContributionRegistry *umi_plugin_host_contributions(UmiPluginHost *host) { return host != NULL ? host->contributions : NULL; }
UmiPermissionSet *umi_plugin_host_grants(UmiPluginHost *host) { return host != NULL ? host->grants : NULL; }
UmiStatus umi_plugin_host_register_manifest(UmiPluginHost *host, const UmiPluginManifest *manifest, const char *manifest_path, UmiPluginPermissionDecision *out_permissions)
{
    UmiStatus status;
    if (host == NULL || manifest == NULL || manifest_path == NULL || out_permissions == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_plugin_permissions_evaluate(manifest, host->grants, out_permissions); if (status != UMI_STATUS_OK) return status;
    return umi_plugin_registry_add(host->registry, manifest, manifest_path);
}
