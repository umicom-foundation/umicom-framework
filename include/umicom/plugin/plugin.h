/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/plugin.h
 *
 * PURPOSE:
 *   Publish the public plugin contract for reusable Umicom applications.
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

typedef struct UmiPluginLibrary UmiPluginLibrary;
typedef const UmiModuleDescriptor *(*UmiModuleQueryFn)(uint32_t requested_abi);

UmiStatus umi_plugin_load(const char *path, UmiPluginLibrary **out_plugin);
void umi_plugin_unload(UmiPluginLibrary *plugin);
const UmiModuleDescriptor *umi_plugin_descriptor(const UmiPluginLibrary *plugin);

#ifdef __cplusplus
}
#endif

#endif
