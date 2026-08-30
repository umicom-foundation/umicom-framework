/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/host.h
 *
 * PURPOSE:
 *   Own plug-in catalogue, contribution registry and permission grants for one
 *   application process.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_HOST_H
#define UMICOM_PLUGIN_HOST_H

#include "umicom/base/status.h"
#include "umicom/plugin/registry.h"
#include "umicom/plugin/contribution.h"
#include "umicom/plugin/permission.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPluginHost UmiPluginHost;
UmiStatus umi_plugin_host_create(UmiPluginHost **out_host);
void umi_plugin_host_destroy(UmiPluginHost *host);
UmiPluginRegistry *umi_plugin_host_registry(UmiPluginHost *host);
UmiPluginContributionRegistry *umi_plugin_host_contributions(UmiPluginHost *host);
UmiPermissionSet *umi_plugin_host_grants(UmiPluginHost *host);
UmiStatus umi_plugin_host_register_manifest(UmiPluginHost *host,
                                            const UmiPluginManifest *manifest,
                                            const char *manifest_path,
                                            UmiPluginPermissionDecision *out_permissions);
#ifdef __cplusplus
}
#endif

#endif
