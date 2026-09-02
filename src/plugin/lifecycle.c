/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/lifecycle.c
 *
 * PURPOSE:
 *   Implement ordered module lifecycle execution with failure state capture and
 *   reverse shutdown.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/lifecycle.h"

#include <stddef.h>
#include <string.h>
/* Provide the call phase operation used by this module and its client applications. */
static UmiStatus call_phase(UmiModulePhaseFn fn, UmiModuleContext *context) { return fn != NULL ? fn(context) : UMI_STATUS_OK; }
/*
 * Initialise plugin lifecycle from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_plugin_lifecycle_init(UmiPluginLifecycle *l, UmiLoadedPlugin *plugin, UmiModuleContext *context)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (l == NULL || plugin == NULL || plugin->descriptor == NULL || context == NULL) return UMI_STATUS_INVALID_ARGUMENT; (void)memset(l, 0, sizeof(*l)); l->plugin = plugin; l->context = context; l->state = UMI_PLUGIN_LOADED; return UMI_STATUS_OK; }
/*
 * Provide the plugin lifecycle start operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_lifecycle_start(UmiPluginLifecycle *l)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (l == NULL || l->plugin == NULL || l->plugin->descriptor == NULL || l->state != UMI_PLUGIN_LOADED) return UMI_STATUS_INVALID_STATE;
    status = call_phase(l->plugin->descriptor->lifecycle.configure, l->context); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) { l->state = UMI_PLUGIN_FAILED; return status; } l->state = UMI_PLUGIN_CONFIGURED;
    status = call_phase(l->plugin->descriptor->lifecycle.initialise, l->context); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) { l->state = UMI_PLUGIN_FAILED; return status; }
    status = call_phase(l->plugin->descriptor->lifecycle.start, l->context); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) { l->state = UMI_PLUGIN_FAILED; return status; } l->state = UMI_PLUGIN_STARTED; return UMI_STATUS_OK;
}
/*
 * Provide the plugin lifecycle quiesce operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_lifecycle_quiesce(UmiPluginLifecycle *l)
{ UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (l == NULL || l->state != UMI_PLUGIN_STARTED) return UMI_STATUS_INVALID_STATE; status = call_phase(l->plugin->descriptor->lifecycle.quiesce, l->context); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (status == UMI_STATUS_OK) l->state = UMI_PLUGIN_QUIESCED; /* Use this fallback path when the earlier condition does not apply. */ else l->state = UMI_PLUGIN_FAILED; return status; }
/*
 * Provide the plugin lifecycle stop operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_lifecycle_stop(UmiPluginLifecycle *l)
{ UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (l == NULL || (l->state != UMI_PLUGIN_STARTED && l->state != UMI_PLUGIN_QUIESCED && l->state != UMI_PLUGIN_FAILED)) return UMI_STATUS_INVALID_STATE; status = call_phase(l->plugin->descriptor->lifecycle.stop, l->context); l->state = status == UMI_STATUS_OK ? UMI_PLUGIN_STOPPED : UMI_PLUGIN_FAILED; return status; }
/*
 * Release or reset state held by plugin lifecycle so the same storage can be reused
 * safely.
 */
void umi_plugin_lifecycle_destroy(UmiPluginLifecycle *l)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (l == NULL) return; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (l->plugin != NULL && l->plugin->descriptor != NULL && l->plugin->descriptor->lifecycle.destroy != NULL) l->plugin->descriptor->lifecycle.destroy(l->context); (void)memset(l, 0, sizeof(*l)); }
