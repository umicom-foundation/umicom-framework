/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/include/umicom/plugin/plugin.h
 *
 * PURPOSE:
 *   Declare the plugin contract shared by Framework services and thin
 *   applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLUGIN_PLUGIN_H
#define UMICOM_PLUGIN_PLUGIN_H

#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/runtime/module.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin library data shared with callers of this public contract.
 */
typedef struct UmiPluginLibrary UmiPluginLibrary;
typedef const UmiModuleDescriptor *(*UmiModuleQueryFn)(uint32_t requested_abi);

/**
 * Read plugin into validated module state and return a status when input cannot be used.
 */
UmiStatus umi_plugin_load(const char *path, UmiPluginLibrary **out_plugin);
/**
 * Provide the plugin unload operation used by this module and its client applications.
 */
void umi_plugin_unload(UmiPluginLibrary *plugin);
/**
 * Provide the plugin descriptor operation used by this module and its client applications.
 */
const UmiModuleDescriptor *umi_plugin_descriptor(const UmiPluginLibrary *plugin);

#ifdef __cplusplus
}
#endif

#endif
