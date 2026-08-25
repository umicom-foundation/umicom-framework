/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/extension_manager_state.c
 *
 * PURPOSE:
 *   Aggregate installation, activation, host and quarantine counts for Extension Centre.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/extension_manager_state.h"
#include <string.h>
void umi_plugin_extension_host_extension_manager_state_init(UmiPluginExtensionHostExtensionManagerState *s) { if(s!=NULL) memset(s,0,sizeof(*s)); }
void umi_plugin_extension_host_extension_manager_state_record(UmiPluginExtensionHostExtensionManagerState *s,UmiPluginExtensionHostLifecycle l,int update) { if(s==NULL) return; ++s->discovered; if(l>=UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_INSTALLED) ++s->installed; if(l==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_ENABLED||l==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STARTING||l==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING||l==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPING) ++s->enabled; if(l==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING) ++s->running; if(l==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_FAILED) ++s->failed; if(l==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_QUARANTINED) ++s->quarantined; if(update) ++s->updates_available; ++s->revision; }
UmiPluginExtensionHostHealthState umi_plugin_extension_host_extension_manager_state_health(const UmiPluginExtensionHostExtensionManagerState *s) { if(s==NULL) return UMI_PLUGIN_EXTENSION_HOST_HEALTH_UNKNOWN; if(s->quarantined>0U||s->failed*2U>s->discovered) return UMI_PLUGIN_EXTENSION_HOST_HEALTH_UNHEALTHY; if(s->failed>0U) return UMI_PLUGIN_EXTENSION_HOST_HEALTH_DEGRADED; return UMI_PLUGIN_EXTENSION_HOST_HEALTH_HEALTHY; }
