/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/manager.h
 *
 * PURPOSE:
 *   Coordinate the existing plug-in host with dependency resolution,
 *   extension points, catalogue, policy, SDK services and audit history.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_MANAGER_H
#define UMICOM_PLUGIN_MANAGER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/plugin/catalogue.h"
#include "umicom/plugin/dependency.h"
#include "umicom/plugin/event.h"
#include "umicom/plugin/extension_point.h"
#include "umicom/plugin/host.h"
#include "umicom/plugin/package.h"
#include "umicom/plugin/policy.h"
#include "umicom/plugin/sdk.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginManager UmiPluginManager;

typedef struct UmiPluginManagerSnapshot {
    uint64_t revision;
    size_t installed;
    size_t enabled;
    size_t active;
    size_t failed;
    size_t contributions;
    size_t extension_points;
    size_t catalogue_entries;
    size_t pending_updates;
    size_t audit_events;
} UmiPluginManagerSnapshot;

UmiStatus umi_plugin_manager_create(UmiPluginHost *host,
                                    const UmiPluginPolicy *policy,
                                    UmiPluginManager **out_manager);
void umi_plugin_manager_destroy(UmiPluginManager *manager);
UmiPluginHost *umi_plugin_manager_host(UmiPluginManager *manager);
UmiPluginExtensionPointRegistry *umi_plugin_manager_extension_points(
    UmiPluginManager *manager);
UmiPluginCatalogue *umi_plugin_manager_catalogue(UmiPluginManager *manager);
UmiPluginEventLog *umi_plugin_manager_events(UmiPluginManager *manager);
UmiPluginSdkServiceRegistry *umi_plugin_manager_services(UmiPluginManager *manager);
UmiStatus umi_plugin_manager_register(UmiPluginManager *manager,
                                      const UmiPluginManifest *manifest,
                                      const char *manifest_path,
                                      UmiPluginTrustLevel trust,
                                      uint64_t timestamp_ms,
                                      UmiPluginPolicyDecision *out_decision);
UmiStatus umi_plugin_manager_set_enabled(UmiPluginManager *manager,
                                         const char *plugin_id,
                                         int enabled,
                                         uint64_t timestamp_ms);
UmiStatus umi_plugin_manager_add_contribution(UmiPluginManager *manager,
                                              const UmiPluginContribution *contribution);
UmiStatus umi_plugin_manager_snapshot(const UmiPluginManager *manager,
                                      UmiPluginManagerSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
