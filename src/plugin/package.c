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

static int path_is_safe_relative(const char *path)
{
    if (path == NULL || path[0] == '\0' || path[0] == '/' || path[0] == '\\') return 0;
    if (strlen(path) >= UMI_PATH_CAPACITY || strstr(path, "..") != NULL) return 0;
    return strchr(path, ':') == NULL;
}

UmiStatus umi_plugin_package_init(UmiPluginPackage *package,
                                  const UmiPluginManifest *manifest,
                                  const char *source_path)
{
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

UmiStatus umi_plugin_package_add_file(UmiPluginPackage *package,
                                      const char *relative_path)
{
    size_t index;
    if (package == NULL || !path_is_safe_relative(relative_path)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < package->file_count; ++index) {
        if (strcmp(package->files[index], relative_path) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (package->file_count >= UMI_PLUGIN_PACKAGE_FILE_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(package->files[package->file_count++], UMI_PATH_CAPACITY,
                   "%s", relative_path);
    return UMI_STATUS_OK;
}

UmiStatus umi_plugin_package_validate(const UmiPluginPackage *package,
                                      char *out_reason,
                                      size_t reason_capacity)
{
    char manifest_reason[256];
    size_t index;
    if (package == NULL || out_reason == NULL || reason_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_plugin_manifest_validate(&package->manifest,
                                     manifest_reason,
                                     sizeof(manifest_reason)) != UMI_STATUS_OK) {
        (void)snprintf(out_reason, reason_capacity, "manifest: %s", manifest_reason);
        return UMI_STATUS_INVALID_STATE;
    }
    if (package->file_count == 0U) {
        (void)snprintf(out_reason, reason_capacity, "package contains no files");
        return UMI_STATUS_INVALID_STATE;
    }
    for (index = 0U; index < package->file_count; ++index) {
        if (!path_is_safe_relative(package->files[index])) {
            (void)snprintf(out_reason, reason_capacity,
                           "unsafe package path: %s", package->files[index]);
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    (void)snprintf(out_reason, reason_capacity, "package inventory is valid");
    return UMI_STATUS_OK;
}

static UmiStatus add_step(UmiPluginPackagePlan *plan, const char *text)
{
    if (plan->step_count >= UMI_PLUGIN_PACKAGE_STEP_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(plan->steps[plan->step_count++],
                   sizeof(plan->steps[0]), "%s", text);
    return UMI_STATUS_OK;
}

UmiStatus umi_plugin_package_plan_create(UmiPluginPackageAction action,
                                         const UmiPluginPackage *package,
                                         UmiVersion installed_version,
                                         const char *install_root,
                                         UmiPluginPackagePlan *out_plan)
{
    char reason[256];
    if (package == NULL || install_root == NULL || install_root[0] == '\0' ||
        out_plan == NULL || strlen(install_root) >= UMI_PATH_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
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
    if (add_step(out_plan, "validate package manifest and inventory") != UMI_STATUS_OK ||
        add_step(out_plan, "verify checksum and publisher trust") != UMI_STATUS_OK ||
        add_step(out_plan, "resolve dependencies and compatibility") != UMI_STATUS_OK ||
        add_step(out_plan, "quiesce active extension instance") != UMI_STATUS_OK ||
        add_step(out_plan, "create recoverable installation checkpoint") != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (action != UMI_PLUGIN_PACKAGE_UNINSTALL) {
        if (add_step(out_plan, "stage package files outside the live directory") != UMI_STATUS_OK ||
            add_step(out_plan, "atomically promote staged package") != UMI_STATUS_OK) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    } else if (add_step(out_plan, "move installed package to recoverable trash") != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (add_step(out_plan, "update extension registry and enablement state") != UMI_STATUS_OK ||
        add_step(out_plan, "record audit evidence and request host refresh") != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

const char *umi_plugin_package_action_text(UmiPluginPackageAction action)
{
    switch (action) {
        case UMI_PLUGIN_PACKAGE_INSTALL: return "install";
        case UMI_PLUGIN_PACKAGE_UPDATE: return "update";
        case UMI_PLUGIN_PACKAGE_ROLLBACK: return "rollback";
        case UMI_PLUGIN_PACKAGE_UNINSTALL: return "uninstall";
        default: return "unknown";
    }
}
