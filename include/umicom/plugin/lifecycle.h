/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/lifecycle.h
 *
 * PURPOSE:
 *   Drive a loaded plug-in module through configure, initialise, start, quiesce,
 *   stop and destroy phases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_LIFECYCLE_H
#define UMICOM_PLUGIN_LIFECYCLE_H

#include "umicom/base/status.h"
#include "umicom/plugin/loader.h"
#include "umicom/plugin/state.h"
#include "umicom/runtime/module.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPluginLifecycle { UmiLoadedPlugin *plugin; UmiModuleContext *context; UmiPluginState state; } UmiPluginLifecycle;
UmiStatus umi_plugin_lifecycle_init(UmiPluginLifecycle *lifecycle,
                                    UmiLoadedPlugin *plugin,
                                    UmiModuleContext *context);
UmiStatus umi_plugin_lifecycle_start(UmiPluginLifecycle *lifecycle);
UmiStatus umi_plugin_lifecycle_quiesce(UmiPluginLifecycle *lifecycle);
UmiStatus umi_plugin_lifecycle_stop(UmiPluginLifecycle *lifecycle);
void umi_plugin_lifecycle_destroy(UmiPluginLifecycle *lifecycle);
#ifdef __cplusplus
}
#endif

#endif
