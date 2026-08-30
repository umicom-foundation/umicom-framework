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
static UmiStatus call_phase(UmiModulePhaseFn fn, UmiModuleContext *context) { return fn != NULL ? fn(context) : UMI_STATUS_OK; }
UmiStatus umi_plugin_lifecycle_init(UmiPluginLifecycle *l, UmiLoadedPlugin *plugin, UmiModuleContext *context)
{ if (l == NULL || plugin == NULL || plugin->descriptor == NULL || context == NULL) return UMI_STATUS_INVALID_ARGUMENT; (void)memset(l, 0, sizeof(*l)); l->plugin = plugin; l->context = context; l->state = UMI_PLUGIN_LOADED; return UMI_STATUS_OK; }
UmiStatus umi_plugin_lifecycle_start(UmiPluginLifecycle *l)
{
    UmiStatus status;
    if (l == NULL || l->plugin == NULL || l->plugin->descriptor == NULL || l->state != UMI_PLUGIN_LOADED) return UMI_STATUS_INVALID_STATE;
    status = call_phase(l->plugin->descriptor->lifecycle.configure, l->context); if (status != UMI_STATUS_OK) { l->state = UMI_PLUGIN_FAILED; return status; } l->state = UMI_PLUGIN_CONFIGURED;
    status = call_phase(l->plugin->descriptor->lifecycle.initialise, l->context); if (status != UMI_STATUS_OK) { l->state = UMI_PLUGIN_FAILED; return status; }
    status = call_phase(l->plugin->descriptor->lifecycle.start, l->context); if (status != UMI_STATUS_OK) { l->state = UMI_PLUGIN_FAILED; return status; } l->state = UMI_PLUGIN_STARTED; return UMI_STATUS_OK;
}
UmiStatus umi_plugin_lifecycle_quiesce(UmiPluginLifecycle *l)
{ UmiStatus status; if (l == NULL || l->state != UMI_PLUGIN_STARTED) return UMI_STATUS_INVALID_STATE; status = call_phase(l->plugin->descriptor->lifecycle.quiesce, l->context); if (status == UMI_STATUS_OK) l->state = UMI_PLUGIN_QUIESCED; else l->state = UMI_PLUGIN_FAILED; return status; }
UmiStatus umi_plugin_lifecycle_stop(UmiPluginLifecycle *l)
{ UmiStatus status; if (l == NULL || (l->state != UMI_PLUGIN_STARTED && l->state != UMI_PLUGIN_QUIESCED && l->state != UMI_PLUGIN_FAILED)) return UMI_STATUS_INVALID_STATE; status = call_phase(l->plugin->descriptor->lifecycle.stop, l->context); l->state = status == UMI_STATUS_OK ? UMI_PLUGIN_STOPPED : UMI_PLUGIN_FAILED; return status; }
void umi_plugin_lifecycle_destroy(UmiPluginLifecycle *l)
{ if (l == NULL) return; if (l->plugin != NULL && l->plugin->descriptor != NULL && l->plugin->descriptor->lifecycle.destroy != NULL) l->plugin->descriptor->lifecycle.destroy(l->context); (void)memset(l, 0, sizeof(*l)); }
