/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/registry.h
 *
 * PURPOSE:
 *   Store plug-in manifests, source paths, enablement and lifecycle state as the
 *   authoritative host catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/**
 * Represent the plugin record data shared with callers of this public contract.
 */
typedef struct UmiPluginRecord { UmiPluginManifest manifest; char manifest_path[UMI_PATH_CAPACITY]; UmiPluginState state; int enabled; } UmiPluginRecord;
/**
 * Represent the plugin registry data shared with callers of this public contract.
 */
typedef struct UmiPluginRegistry UmiPluginRegistry;
/**
 * Initialise plugin registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_plugin_registry_create(UmiPluginRegistry **out_registry);
/**
 * Release or reset state held by plugin registry so the same storage can be reused safely.
 */
void umi_plugin_registry_destroy(UmiPluginRegistry *registry);
/**
 * Add plugin registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_plugin_registry_add(UmiPluginRegistry *registry,
                                  const UmiPluginManifest *manifest,
                                  const char *manifest_path);
/**
 * Provide the plugin registry get operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_registry_get(const UmiPluginRegistry *registry,
                                  const char *plugin_id,
                                  UmiPluginRecord *out_record);
/**
 * Provide the plugin registry set enabled operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_registry_set_enabled(UmiPluginRegistry *registry,
                                          const char *plugin_id,
                                          int enabled);
/**
 * Provide the plugin registry transition operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_registry_transition(UmiPluginRegistry *registry,
                                         const char *plugin_id,
                                         UmiPluginState state);
/**
 * Return the number of records represented by plugin registry without changing their
 * state.
 */
size_t umi_plugin_registry_count(const UmiPluginRegistry *registry);
/**
 * Find plugin registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_plugin_registry_at(const UmiPluginRegistry *registry,
                                 size_t index,
                                 UmiPluginRecord *out_record);
#ifdef __cplusplus
}
#endif

#endif
