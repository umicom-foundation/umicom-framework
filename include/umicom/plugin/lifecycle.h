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
/**
 * Represent the plugin lifecycle data shared with callers of this public contract.
 */
typedef struct UmiPluginLifecycle { UmiLoadedPlugin *plugin; UmiModuleContext *context; UmiPluginState state; } UmiPluginLifecycle;
/**
 * Initialise plugin lifecycle from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_plugin_lifecycle_init(UmiPluginLifecycle *lifecycle,
                                    UmiLoadedPlugin *plugin,
                                    UmiModuleContext *context);
/**
 * Provide the plugin lifecycle start operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_lifecycle_start(UmiPluginLifecycle *lifecycle);
/**
 * Provide the plugin lifecycle quiesce operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_lifecycle_quiesce(UmiPluginLifecycle *lifecycle);
/**
 * Provide the plugin lifecycle stop operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_lifecycle_stop(UmiPluginLifecycle *lifecycle);
/**
 * Release or reset state held by plugin lifecycle so the same storage can be reused
 * safely.
 */
void umi_plugin_lifecycle_destroy(UmiPluginLifecycle *lifecycle);
#ifdef __cplusplus
}
#endif

#endif
