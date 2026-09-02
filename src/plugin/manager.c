/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/manager.c
 *
 * PURPOSE:
 *   Coordinate the existing plug-in host with the Extension SDK v2 policy,
 *   contribution, catalogue, service and audit contracts.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/plugin/manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiPluginManager {
    UmiPluginHost *host;
    UmiPluginPolicy policy;
    UmiPluginExtensionPointRegistry *extension_points;
    UmiPluginCatalogue *catalogue;
    UmiPluginEventLog *events;
    UmiPluginSdkServiceRegistry *services;
    uint64_t revision;
    int owns_host;
};

/*
 * Initialise plugin manager from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_plugin_manager_create(UmiPluginHost *host,
                                    const UmiPluginPolicy *policy,
                                    UmiPluginManager **out_manager)
{
    UmiPluginManager *manager;
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_manager == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_manager = NULL;
    manager = (UmiPluginManager *)calloc(1U, sizeof(*manager));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    manager->policy = policy != NULL ? *policy : umi_plugin_policy_default();
    manager->host = host;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager->host == NULL) {
        status = umi_plugin_host_create(&manager->host);
        manager->owns_host = status == UMI_STATUS_OK;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_plugin_extension_point_registry_create(&manager->extension_points);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_plugin_catalogue_create(&manager->catalogue);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_plugin_event_log_create(256U, &manager->events);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_plugin_sdk_service_registry_create(&manager->services);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_plugin_manager_destroy(manager);
        return status;
    }
    manager->revision = 1U;
    *out_manager = manager;
    return UMI_STATUS_OK;
}

/* Release or reset state held by plugin manager so the same storage can be reused safely. */
void umi_plugin_manager_destroy(UmiPluginManager *manager)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL) return;
    umi_plugin_sdk_service_registry_destroy(manager->services);
    umi_plugin_event_log_destroy(manager->events);
    umi_plugin_catalogue_destroy(manager->catalogue);
    umi_plugin_extension_point_registry_destroy(manager->extension_points);
    /* Apply this branch only when its contract condition is satisfied. */
    if (manager->owns_host) umi_plugin_host_destroy(manager->host);
    free(manager);
}

/*
 * Provide the plugin manager host operation used by this module and its client
 * applications.
 */
UmiPluginHost *umi_plugin_manager_host(UmiPluginManager *manager)
{
    return manager != NULL ? manager->host : NULL;
}

/*
 * Provide the plugin manager extension points operation used by this module and its client
 * applications.
 */
UmiPluginExtensionPointRegistry *umi_plugin_manager_extension_points(
    UmiPluginManager *manager)
{
    return manager != NULL ? manager->extension_points : NULL;
}

/*
 * Provide the plugin manager catalogue operation used by this module and its client
 * applications.
 */
UmiPluginCatalogue *umi_plugin_manager_catalogue(UmiPluginManager *manager)
{
    return manager != NULL ? manager->catalogue : NULL;
}

/*
 * Provide the plugin manager events operation used by this module and its client
 * applications.
 */
UmiPluginEventLog *umi_plugin_manager_events(UmiPluginManager *manager)
{
    return manager != NULL ? manager->events : NULL;
}

/*
 * Provide the plugin manager services operation used by this module and its client
 * applications.
 */
UmiPluginSdkServiceRegistry *umi_plugin_manager_services(UmiPluginManager *manager)
{
    return manager != NULL ? manager->services : NULL;
}

/* Add plugin manager only after its inputs and available capacity have been checked. */
UmiStatus umi_plugin_manager_register(UmiPluginManager *manager,
                                      const UmiPluginManifest *manifest,
                                      const char *manifest_path,
                                      UmiPluginTrustLevel trust,
                                      uint64_t timestamp_ms,
                                      UmiPluginPolicyDecision *out_decision)
{
    UmiPluginPermissionDecision permissions;
    UmiPluginSignatureDecision signature;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL || manifest == NULL || manifest_path == NULL ||
        out_decision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(&signature, 0, sizeof(signature));
    (void)snprintf(signature.reason, sizeof(signature.reason),
                   "no package signature supplied");
    status = umi_plugin_permissions_evaluate(
        manifest, umi_plugin_host_grants(manager->host), &permissions);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_plugin_policy_evaluate(&manager->policy,
                                            manifest,
                                            &signature,
                                            &permissions,
                                            trust,
                                            out_decision);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_plugin_registry_add(umi_plugin_host_registry(manager->host),
                                         manifest, manifest_path);
    }
    (void)umi_plugin_event_log_append(
        manager->events,
        timestamp_ms,
        status == UMI_STATUS_OK ? UMI_PLUGIN_EVENT_REGISTERED
                                : UMI_PLUGIN_EVENT_FAILED,
        status,
        manifest->plugin_id,
        status == UMI_STATUS_OK ? out_decision->reason : "registration rejected");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++manager->revision;
    return status;
}

/*
 * Provide the plugin manager set enabled operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_manager_set_enabled(UmiPluginManager *manager,
                                         const char *plugin_id,
                                         int enabled,
                                         uint64_t timestamp_ms)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL || plugin_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_plugin_registry_set_enabled(
        umi_plugin_host_registry(manager->host), plugin_id, enabled);
    (void)umi_plugin_event_log_append(
        manager->events,
        timestamp_ms,
        status == UMI_STATUS_OK
            ? (enabled ? UMI_PLUGIN_EVENT_ENABLED : UMI_PLUGIN_EVENT_DISABLED)
            : UMI_PLUGIN_EVENT_FAILED,
        status,
        plugin_id,
        status == UMI_STATUS_OK
            ? (enabled ? "extension enabled" : "extension disabled")
            : "enablement update failed");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++manager->revision;
    return status;
}

/*
 * Provide the plugin manager add contribution operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_manager_add_contribution(UmiPluginManager *manager,
                                              const UmiPluginContribution *contribution)
{
    char reason[256];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL || contribution == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_plugin_extension_point_validate_contribution(
        manager->extension_points,
        umi_plugin_host_contributions(manager->host),
        contribution,
        reason,
        sizeof(reason));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_plugin_contribution_registry_add(
        umi_plugin_host_contributions(manager->host), contribution);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++manager->revision;
    return status;
}

/*
 * Provide the plugin manager snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_manager_snapshot(const UmiPluginManager *manager,
                                      UmiPluginManagerSnapshot *out_snapshot)
{
    UmiPluginRegistry *registry;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->revision = manager->revision;
    registry = umi_plugin_host_registry(manager->host);
    out_snapshot->installed = umi_plugin_registry_count(registry);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < out_snapshot->installed; ++index) {
        UmiPluginRecord record;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_plugin_registry_at(registry, index, &record) != UMI_STATUS_OK) continue;
        /* Apply this operation only while the related capability or state is available. */
        if (record.enabled) ++out_snapshot->enabled;
        /* Apply this operation only while the related capability or state is available. */
        if (record.state == UMI_PLUGIN_STARTED) ++out_snapshot->active;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (record.state == UMI_PLUGIN_FAILED) ++out_snapshot->failed;
        {
            size_t catalogue_index;
            size_t catalogue_count = umi_plugin_catalogue_count(manager->catalogue);
            /* Visit each bounded item once so every record receives the same rule. */
            for (catalogue_index = 0U; catalogue_index < catalogue_count;
                 ++catalogue_index) {
                UmiPluginCatalogueEntry entry;
                /* Apply this branch only when its contract condition is satisfied. */
                if (umi_plugin_catalogue_at(manager->catalogue,
                                            catalogue_index,
                                            &entry) == UMI_STATUS_OK &&
                    strcmp(entry.plugin_id, record.manifest.plugin_id) == 0 &&
                    umi_plugin_catalogue_update_available(
                        &entry, record.manifest.version)) {
                    ++out_snapshot->pending_updates;
                    break;
                }
            }
        }
    }
    out_snapshot->contributions = umi_plugin_contribution_registry_count(
        umi_plugin_host_contributions(manager->host));
    out_snapshot->extension_points = umi_plugin_extension_point_registry_count(
        manager->extension_points);
    out_snapshot->catalogue_entries = umi_plugin_catalogue_count(manager->catalogue);
    out_snapshot->audit_events = umi_plugin_event_log_count(manager->events);
    return UMI_STATUS_OK;
}
