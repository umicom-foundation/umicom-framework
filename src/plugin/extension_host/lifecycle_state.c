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
void umi_plugin_extension_host_lifecycle_state_init(UmiPluginExtensionHostLifecycleState *v) { if(v!=NULL) { v->state=UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_DISCOVERED; v->previous=v->state; v->revision=0U; v->changed_ms=0U; } }
UmiStatus umi_plugin_extension_host_lifecycle_state_set(UmiPluginExtensionHostLifecycleState *v,UmiPluginExtensionHostLifecycle next,uint64_t now) { if(v==NULL||next<UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_DISCOVERED||next>UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_QUARANTINED) return UMI_STATUS_INVALID_ARGUMENT; v->previous=v->state; v->state=next; v->changed_ms=now; ++v->revision; return UMI_STATUS_OK; }
int umi_plugin_extension_host_lifecycle_state_active(const UmiPluginExtensionHostLifecycleState *v) { return v!=NULL&&(v->state==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STARTING||v->state==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING||v->state==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPING); }
