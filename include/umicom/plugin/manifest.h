/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/manifest.h
 *
 * PURPOSE:
 *   Define and parse inspectable plug-in manifests containing identity, ABI,
 *   library, permissions and capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_MANIFEST_H
#define UMICOM_PLUGIN_MANIFEST_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include "umicom/platform/path.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_PLUGIN_ID_CAPACITY 128U
#define UMI_PLUGIN_NAME_CAPACITY 160U
#define UMI_PLUGIN_LIST_MAX 32U
#define UMI_PLUGIN_ITEM_CAPACITY 160U
typedef struct UmiPluginManifest {
    char plugin_id[UMI_PLUGIN_ID_CAPACITY];
    char display_name[UMI_PLUGIN_NAME_CAPACITY];
    UmiVersion version;
    uint32_t required_abi;
    char library_path[UMI_PATH_CAPACITY];
    char permissions[UMI_PLUGIN_LIST_MAX][UMI_PLUGIN_ITEM_CAPACITY];
    size_t permission_count;
    char capabilities[UMI_PLUGIN_LIST_MAX][UMI_PLUGIN_ITEM_CAPACITY];
    size_t capability_count;
} UmiPluginManifest;
UmiStatus umi_plugin_manifest_parse(const char *text, UmiPluginManifest *out_manifest);
UmiStatus umi_plugin_manifest_load(const char *path, UmiPluginManifest *out_manifest);
UmiStatus umi_plugin_manifest_validate(const UmiPluginManifest *manifest,
                                       char *out_reason,
                                       size_t reason_capacity);
#ifdef __cplusplus
}
#endif

#endif
