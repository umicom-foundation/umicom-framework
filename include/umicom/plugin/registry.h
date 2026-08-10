/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/registry.h
 *
 * PURPOSE:
 *   Store plug-in manifests, source paths, enablement and lifecycle state as the
 *   authoritative host catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_REGISTRY_H
#define UMICOM_PLUGIN_REGISTRY_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/plugin/manifest.h"
#include "umicom/plugin/state.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_PLUGIN_REGISTRY_MAX 512U
typedef struct UmiPluginRecord { UmiPluginManifest manifest; char manifest_path[UMI_PATH_CAPACITY]; UmiPluginState state; int enabled; } UmiPluginRecord;
typedef struct UmiPluginRegistry UmiPluginRegistry;
UmiStatus umi_plugin_registry_create(UmiPluginRegistry **out_registry);
void umi_plugin_registry_destroy(UmiPluginRegistry *registry);
UmiStatus umi_plugin_registry_add(UmiPluginRegistry *registry,
                                  const UmiPluginManifest *manifest,
                                  const char *manifest_path);
UmiStatus umi_plugin_registry_get(const UmiPluginRegistry *registry,
                                  const char *plugin_id,
                                  UmiPluginRecord *out_record);
UmiStatus umi_plugin_registry_set_enabled(UmiPluginRegistry *registry,
                                          const char *plugin_id,
                                          int enabled);
UmiStatus umi_plugin_registry_transition(UmiPluginRegistry *registry,
                                         const char *plugin_id,
                                         UmiPluginState state);
size_t umi_plugin_registry_count(const UmiPluginRegistry *registry);
UmiStatus umi_plugin_registry_at(const UmiPluginRegistry *registry,
                                 size_t index,
                                 UmiPluginRecord *out_record);
#ifdef __cplusplus
}
#endif

#endif
