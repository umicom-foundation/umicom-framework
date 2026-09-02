/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/loader.h
 *
 * PURPOSE:
 *   Combine manifest validation, compatibility and dynamic-library loading into
 *   one explicit operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_LOADER_H
#define UMICOM_PLUGIN_LOADER_H

#include "umicom/base/status.h"
#include "umicom/plugin/plugin.h"
#include "umicom/plugin/manifest.h"
#include "umicom/plugin/compatibility.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the loaded plugin data shared with callers of this public contract.
 */
typedef struct UmiLoadedPlugin { UmiPluginManifest manifest; UmiPluginLibrary *library; const UmiModuleDescriptor *descriptor; } UmiLoadedPlugin;
/**
 * Read plugin loader into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_plugin_loader_load(const UmiPluginManifest *manifest,
                                 UmiLoadedPlugin *out_plugin,
                                 char *out_reason,
                                 size_t reason_capacity);
/**
 * Provide the plugin loader unload operation used by this module and its client
 * applications.
 */
void umi_plugin_loader_unload(UmiLoadedPlugin *plugin);
#ifdef __cplusplus
}
#endif

#endif
