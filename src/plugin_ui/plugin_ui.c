/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin_ui/plugin_ui.c
 *
 * PURPOSE:
 *   Build toolkit-neutral Extension Centre projections and executable command
 *   metadata while keeping all GTK objects outside public contracts.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/plugin_ui/plugin_ui.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "umicom/ui/command_view.h"

/* Provide the set string operation used by this module and its client applications. */
static UmiStatus set_string(UmiUiViewModel *view,
                            const char *key,
                            const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(&value, text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

/* Provide the set integer operation used by this module and its client applications. */
static UmiStatus set_integer(UmiUiViewModel *view,
                             const char *key,
                             int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

/* Provide the set action operation used by this module and its client applications. */
static UmiStatus set_action(UmiUiViewModel *view,
                            size_t index,
                            const char *action_id,
                            const char *label,
                            const char *tooltip,
                            int enabled)
{
    UmiUiCommandViewAction action;
    (void)memset(&action, 0, sizeof(action));
    (void)snprintf(action.action_id, sizeof(action.action_id), "%s", action_id);
    (void)snprintf(action.label, sizeof(action.label), "%s", label);
    (void)snprintf(action.tooltip, sizeof(action.tooltip), "%s", tooltip);
    action.enabled = enabled != 0;
    return umi_ui_command_view_set_action(view, index, &action);
}

/* Provide the base view operation used by this module and its client applications. */
static UmiStatus base_view(const char *view_id,
                           const char *kind,
                           const char *title,
                           const char *summary,
                           UmiUiViewModel **out_view)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_view = NULL;
    status = umi_ui_view_model_create(view_id, "umicom.plugin-ui",
                                      UMI_UI_ROLE_PANE, out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "umicom.view-kind", kind);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "title", title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "summary", summary);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

/*
 * Initialise plugin ui installed view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_plugin_ui_installed_view_create(const char *view_id,
                                              UmiPluginManager *manager,
                                              UmiUiViewModel **out_view)
{
    UmiPluginManagerSnapshot snapshot;
    UmiPluginRegistry *registry;
    size_t count;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(view_id, "extensions-installed", "Installed Extensions",
                       "Installed, enabled and active extensions managed by the Framework host.",
                       out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_plugin_manager_snapshot(manager, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(*out_view, "extensions.installed", (int64_t)snapshot.installed);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(*out_view, "extensions.enabled", (int64_t)snapshot.enabled);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(*out_view, "extensions.active", (int64_t)snapshot.active);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(*out_view, "extensions.failed", (int64_t)snapshot.failed);
    registry = umi_plugin_host_registry(umi_plugin_manager_host(manager));
    count = snapshot.installed < UMI_PLUGIN_UI_VISIBLE_ROWS
        ? snapshot.installed : UMI_PLUGIN_UI_VISIBLE_ROWS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK) status = set_integer(*out_view, "extensions.row-count", (int64_t)count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiPluginRecord record;
        char key[96];
        char text[512];
        status = umi_plugin_registry_at(registry, index, &record);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "extensions.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%s %u.%u.%u | %s | %s",
                       record.manifest.display_name,
                       (unsigned int)record.manifest.version.major,
                       (unsigned int)record.manifest.version.minor,
                       (unsigned int)record.manifest.version.patch,
                       record.enabled ? "enabled" : "disabled",
                       umi_plugin_state_text(record.state));
        status = set_string(*out_view, key, text);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(*out_view, 0U, "studio.action.extensions.refresh", "Refresh", "Refresh installed extension state", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(*out_view, 1U, "studio.action.pane.extension-catalogue", "Catalogue", "Open the compatible extension catalogue", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(*out_view, 2U, "studio.action.pane.extension-permissions", "Permissions", "Review extension permissions and isolation", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(*out_view, 3U, "studio.action.pane.extension-audit", "Audit", "Open extension-management audit history", 1);
    return status;
}

/*
 * Initialise plugin ui catalogue view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_plugin_ui_catalogue_view_create(const char *view_id,
                                              UmiPluginManager *manager,
                                              UmiUiViewModel **out_view)
{
    UmiPluginCatalogue *catalogue;
    size_t count;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(view_id, "extensions-catalogue", "Extension Catalogue",
                       "Provider-neutral extension discovery with compatibility and trust evidence.",
                       out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    catalogue = umi_plugin_manager_catalogue(manager);
    count = umi_plugin_catalogue_count(catalogue);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_PLUGIN_UI_VISIBLE_ROWS) count = UMI_PLUGIN_UI_VISIBLE_ROWS;
    status = set_integer(*out_view, "catalogue.row-count", (int64_t)count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiPluginCatalogueEntry entry;
        char key[96];
        char text[768];
        status = umi_plugin_catalogue_at(catalogue, index, &entry);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "catalogue.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%s %u.%u.%u | %s | %s",
                       entry.display_name,
                       (unsigned int)entry.version.major,
                       (unsigned int)entry.version.minor,
                       (unsigned int)entry.version.patch,
                       entry.publisher,
                       entry.verified ? "verified" : "unverified");
        status = set_string(*out_view, key, text);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(*out_view, 0U, "studio.action.extensions.refresh", "Refresh", "Refresh configured extension catalogues", 1);
    return status;
}

/*
 * Initialise plugin ui permissions view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_plugin_ui_permissions_view_create(const char *view_id,
                                                UmiPluginManager *manager,
                                                UmiUiViewModel **out_view)
{
    UmiPermissionSet *grants;
    size_t count;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(view_id, "extensions-permissions", "Extension Permissions",
                       "Explicit host grants used before extension code can activate.",
                       out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    grants = umi_plugin_host_grants(umi_plugin_manager_host(manager));
    count = umi_permission_set_count(grants);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_PLUGIN_UI_VISIBLE_ROWS) count = UMI_PLUGIN_UI_VISIBLE_ROWS;
    status = set_integer(*out_view, "permissions.row-count", (int64_t)count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        char key[96];
        char permission[UMI_PERMISSION_CAPACITY];
        status = umi_permission_set_at(grants, index, permission, sizeof(permission));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "permissions.row.%zu", index);
        status = set_string(*out_view, key, permission);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(*out_view, 0U, "studio.action.pane.extension-permissions", "Review Grants", "Review extension permissions and workspace trust", 1);
    return status;
}

/*
 * Initialise plugin ui audit view from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_plugin_ui_audit_view_create(const char *view_id,
                                          UmiPluginManager *manager,
                                          UmiUiViewModel **out_view)
{
    UmiPluginEventLog *events;
    size_t total;
    size_t count;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(view_id, "extensions-audit", "Extension Audit",
                       "Chronological evidence for discovery, policy, activation and package operations.",
                       out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    events = umi_plugin_manager_events(manager);
    total = umi_plugin_event_log_count(events);
    count = total < UMI_PLUGIN_UI_VISIBLE_ROWS ? total : UMI_PLUGIN_UI_VISIBLE_ROWS;
    status = set_integer(*out_view, "extension-audit.row-count", (int64_t)count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiPluginEvent event;
        char key[96];
        char text[512];
        status = umi_plugin_event_log_at(events, total - count + index, &event);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "extension-audit.row.%zu", index);
        (void)snprintf(text, sizeof(text), "#%" PRIu64 " %s | %s | %s",
                       event.sequence, event.plugin_id,
                       umi_plugin_event_kind_text(event.kind), event.message);
        status = set_string(*out_view, key, text);
    }
    return status;
}
