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
/**
 * Represent the plugin host data shared with callers of this public contract.
 */
typedef struct UmiPluginHost UmiPluginHost;
/**
 * Initialise plugin host from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_plugin_host_create(UmiPluginHost **out_host);
/**
 * Release or reset state held by plugin host so the same storage can be reused safely.
 */
void umi_plugin_host_destroy(UmiPluginHost *host);
/**
 * Provide the plugin host registry operation used by this module and its client
 * applications.
 */
UmiPluginRegistry *umi_plugin_host_registry(UmiPluginHost *host);
/**
 * Provide the plugin host contributions operation used by this module and its client
 * applications.
 */
UmiPluginContributionRegistry *umi_plugin_host_contributions(UmiPluginHost *host);
/**
 * Provide the plugin host grants operation used by this module and its client
 * applications.
 */
UmiPermissionSet *umi_plugin_host_grants(UmiPluginHost *host);
/**
 * Provide the plugin host register manifest operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_host_register_manifest(UmiPluginHost *host,
                                            const UmiPluginManifest *manifest,
                                            const char *manifest_path,
                                            UmiPluginPermissionDecision *out_permissions);
#ifdef __cplusplus
}
#endif

#endif
