/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/lifecycle_state.c
 *
 * PURPOSE:
 *   Track the extension lifecycle without allowing invalid state regression.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/lifecycle_state.h"
/*
 * Initialise plugin extension host lifecycle state from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_lifecycle_state_init(UmiPluginExtensionHostLifecycleState *v) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(v!=NULL) { v->state=UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_DISCOVERED; v->previous=v->state; v->revision=0U; v->changed_ms=0U; } }
/*
 * Copy plugin extension host lifecycle state into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_plugin_extension_host_lifecycle_state_set(UmiPluginExtensionHostLifecycleState *v,UmiPluginExtensionHostLifecycle next,uint64_t now) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(v==NULL||next<UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_DISCOVERED||next>UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_QUARANTINED) return UMI_STATUS_INVALID_ARGUMENT; v->previous=v->state; v->state=next; v->changed_ms=now; ++v->revision; return UMI_STATUS_OK; }
/*
 * Provide the plugin extension host lifecycle state active operation used by this module
 * and its client applications.
 */
int umi_plugin_extension_host_lifecycle_state_active(const UmiPluginExtensionHostLifecycleState *v) { return v!=NULL&&(v->state==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STARTING||v->state==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING||v->state==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPING); }
