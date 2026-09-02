/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/package.h
 *
 * PURPOSE:
 *   Model an installable plug-in package and construct auditable install,
 *   update, rollback and uninstall plans without performing hidden writes.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_PACKAGE_H
#define UMICOM_PLUGIN_PACKAGE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include "umicom/platform/path.h"
#include "umicom/plugin/manifest.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLUGIN_PACKAGE_FILE_MAX 128U
#define UMI_PLUGIN_PACKAGE_STEP_MAX 32U

/**
 * List the named plugin package action values accepted by this public contract.
 */
typedef enum UmiPluginPackageAction {
    UMI_PLUGIN_PACKAGE_INSTALL = 0,
    UMI_PLUGIN_PACKAGE_UPDATE = 1,
    UMI_PLUGIN_PACKAGE_ROLLBACK = 2,
    UMI_PLUGIN_PACKAGE_UNINSTALL = 3
} UmiPluginPackageAction;

/**
 * Represent the plugin package data shared with callers of this public contract.
 */
typedef struct UmiPluginPackage {
    UmiPluginManifest manifest;
    char source_path[UMI_PATH_CAPACITY];
    char files[UMI_PLUGIN_PACKAGE_FILE_MAX][UMI_PATH_CAPACITY];
    size_t file_count;
    uint64_t checksum;
    uint64_t unpacked_size;
} UmiPluginPackage;

/**
 * Represent the plugin package plan data shared with callers of this public contract.
 */
typedef struct UmiPluginPackagePlan {
    UmiPluginPackageAction action;
    char plugin_id[UMI_PLUGIN_ID_CAPACITY];
    UmiVersion from_version;
    UmiVersion to_version;
    char install_root[UMI_PATH_CAPACITY];
    char staging_root[UMI_PATH_CAPACITY];
    char steps[UMI_PLUGIN_PACKAGE_STEP_MAX][256];
    size_t step_count;
    int requires_restart;
    int reversible;
} UmiPluginPackagePlan;

/**
 * Initialise plugin package from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_plugin_package_init(UmiPluginPackage *package,
                                  const UmiPluginManifest *manifest,
                                  const char *source_path);
/**
 * Provide the plugin package add file operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_package_add_file(UmiPluginPackage *package,
                                      const char *relative_path);
/**
 * Check that plugin package satisfies its contract before another service relies on it.
 */
UmiStatus umi_plugin_package_validate(const UmiPluginPackage *package,
                                      char *out_reason,
                                      size_t reason_capacity);
/**
 * Initialise plugin package plan from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_plugin_package_plan_create(UmiPluginPackageAction action,
                                         const UmiPluginPackage *package,
                                         UmiVersion installed_version,
                                         const char *install_root,
                                         UmiPluginPackagePlan *out_plan);
/**
 * Provide the plugin package action text operation used by this module and its client
 * applications.
 */
const char *umi_plugin_package_action_text(UmiPluginPackageAction action);

#ifdef __cplusplus
}
#endif

#endif
