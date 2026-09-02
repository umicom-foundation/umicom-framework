/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/package.c
 *
 * PURPOSE:
 *   Validate bounded extension package inventories and generate reversible,
 *   inspectable management plans.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/plugin/package.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the path is safe relative operation used by this module and its client
 * applications.
 */
static int path_is_safe_relative(const char *path)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || path[0] == '\0' || path[0] == '/' || path[0] == '\\') return 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strlen(path) >= UMI_PATH_CAPACITY || strstr(path, "..") != NULL) return 0;
    return strchr(path, ':') == NULL;
}

/*
 * Initialise plugin package from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_plugin_package_init(UmiPluginPackage *package,
                                  const UmiPluginManifest *manifest,
                                  const char *source_path)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (package == NULL || manifest == NULL || source_path == NULL ||
        source_path[0] == '\0' || strlen(source_path) >= UMI_PATH_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(package, 0, sizeof(*package));
    package->manifest = *manifest;
    (void)snprintf(package->source_path, sizeof(package->source_path),
                   "%s", source_path);
    return UMI_STATUS_OK;
}

/*
 * Provide the plugin package add file operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_package_add_file(UmiPluginPackage *package,
                                      const char *relative_path)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (package == NULL || !path_is_safe_relative(relative_path)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < package->file_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(package->files[index], relative_path) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (package->file_count >= UMI_PLUGIN_PACKAGE_FILE_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(package->files[package->file_count++], UMI_PATH_CAPACITY,
                   "%s", relative_path);
    return UMI_STATUS_OK;
}

/* Check that plugin package satisfies its contract before another service relies on it. */
UmiStatus umi_plugin_package_validate(const UmiPluginPackage *package,
                                      char *out_reason,
                                      size_t reason_capacity)
{
    char manifest_reason[256];
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (package == NULL || out_reason == NULL || reason_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (umi_plugin_manifest_validate(&package->manifest,
                                     manifest_reason,
                                     sizeof(manifest_reason)) != UMI_STATUS_OK) {
        (void)snprintf(out_reason, reason_capacity, "manifest: %s", manifest_reason);
        return UMI_STATUS_INVALID_STATE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (package->file_count == 0U) {
        (void)snprintf(out_reason, reason_capacity, "package contains no files");
        return UMI_STATUS_INVALID_STATE;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < package->file_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!path_is_safe_relative(package->files[index])) {
            (void)snprintf(out_reason, reason_capacity,
                           "unsafe package path: %s", package->files[index]);
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    (void)snprintf(out_reason, reason_capacity, "package inventory is valid");
    return UMI_STATUS_OK;
}

/* Provide the add step operation used by this module and its client applications. */
static UmiStatus add_step(UmiPluginPackagePlan *plan, const char *text)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->step_count >= UMI_PLUGIN_PACKAGE_STEP_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(plan->steps[plan->step_count++],
                   sizeof(plan->steps[0]), "%s", text);
    return UMI_STATUS_OK;
}

/*
 * Initialise plugin package plan from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_plugin_package_plan_create(UmiPluginPackageAction action,
                                         const UmiPluginPackage *package,
                                         UmiVersion installed_version,
                                         const char *install_root,
                                         UmiPluginPackagePlan *out_plan)
{
    char reason[256];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (package == NULL || install_root == NULL || install_root[0] == '\0' ||
        out_plan == NULL || strlen(install_root) >= UMI_PATH_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_plugin_package_validate(package, reason, sizeof(reason)) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->action = action;
    out_plan->from_version = installed_version;
    out_plan->to_version = package->manifest.version;
    out_plan->requires_restart = 1;
    out_plan->reversible = 1;
    (void)snprintf(out_plan->plugin_id, sizeof(out_plan->plugin_id),
                   "%s", package->manifest.plugin_id);
    (void)snprintf(out_plan->install_root, sizeof(out_plan->install_root),
                   "%s", install_root);
    (void)snprintf(out_plan->staging_root, sizeof(out_plan->staging_root),
                   "%s/.staging/%s", install_root, package->manifest.plugin_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (add_step(out_plan, "validate package manifest and inventory") != UMI_STATUS_OK ||
        add_step(out_plan, "verify checksum and publisher trust") != UMI_STATUS_OK ||
        add_step(out_plan, "resolve dependencies and compatibility") != UMI_STATUS_OK ||
        add_step(out_plan, "quiesce active extension instance") != UMI_STATUS_OK ||
        add_step(out_plan, "create recoverable installation checkpoint") != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (action != UMI_PLUGIN_PACKAGE_UNINSTALL) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (add_step(out_plan, "stage package files outside the live directory") != UMI_STATUS_OK ||
            add_step(out_plan, "atomically promote staged package") != UMI_STATUS_OK) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (add_step(out_plan, "move installed package to recoverable trash") != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (add_step(out_plan, "update extension registry and enablement state") != UMI_STATUS_OK ||
        add_step(out_plan, "record audit evidence and request host refresh") != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the plugin package action text operation used by this module and its client
 * applications.
 */
const char *umi_plugin_package_action_text(UmiPluginPackageAction action)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (action) {
        case UMI_PLUGIN_PACKAGE_INSTALL: return "install";
        case UMI_PLUGIN_PACKAGE_UPDATE: return "update";
        case UMI_PLUGIN_PACKAGE_ROLLBACK: return "rollback";
        case UMI_PLUGIN_PACKAGE_UNINSTALL: return "uninstall";
        default: return "unknown";
    }
}
