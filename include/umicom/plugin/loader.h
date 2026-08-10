/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/loader.h
 *
 * PURPOSE:
 *   Combine manifest validation, compatibility and dynamic-library loading into
 *   one explicit operation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiLoadedPlugin { UmiPluginManifest manifest; UmiPluginLibrary *library; const UmiModuleDescriptor *descriptor; } UmiLoadedPlugin;
UmiStatus umi_plugin_loader_load(const UmiPluginManifest *manifest,
                                 UmiLoadedPlugin *out_plugin,
                                 char *out_reason,
                                 size_t reason_capacity);
void umi_plugin_loader_unload(UmiLoadedPlugin *plugin);
#ifdef __cplusplus
}
#endif

#endif
