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

typedef enum UmiPluginPackageAction {
    UMI_PLUGIN_PACKAGE_INSTALL = 0,
    UMI_PLUGIN_PACKAGE_UPDATE = 1,
    UMI_PLUGIN_PACKAGE_ROLLBACK = 2,
    UMI_PLUGIN_PACKAGE_UNINSTALL = 3
} UmiPluginPackageAction;

typedef struct UmiPluginPackage {
    UmiPluginManifest manifest;
    char source_path[UMI_PATH_CAPACITY];
    char files[UMI_PLUGIN_PACKAGE_FILE_MAX][UMI_PATH_CAPACITY];
    size_t file_count;
    uint64_t checksum;
    uint64_t unpacked_size;
} UmiPluginPackage;

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

UmiStatus umi_plugin_package_init(UmiPluginPackage *package,
                                  const UmiPluginManifest *manifest,
                                  const char *source_path);
UmiStatus umi_plugin_package_add_file(UmiPluginPackage *package,
                                      const char *relative_path);
UmiStatus umi_plugin_package_validate(const UmiPluginPackage *package,
                                      char *out_reason,
                                      size_t reason_capacity);
UmiStatus umi_plugin_package_plan_create(UmiPluginPackageAction action,
                                         const UmiPluginPackage *package,
                                         UmiVersion installed_version,
                                         const char *install_root,
                                         UmiPluginPackagePlan *out_plan);
const char *umi_plugin_package_action_text(UmiPluginPackageAction action);

#ifdef __cplusplus
}
#endif

#endif
