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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/extension_manager_state.h"
#include <string.h>
/*
 * Initialise plugin extension host extension manager state from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_extension_manager_state_init(UmiPluginExtensionHostExtensionManagerState *s) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL) memset(s,0,sizeof(*s)); }
/*
 * Provide the plugin extension host extension manager state record operation used by this
 * module and its client applications.
 */
void umi_plugin_extension_host_extension_manager_state_record(UmiPluginExtensionHostExtensionManagerState *s,UmiPluginExtensionHostLifecycle l,int update) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL) return; ++s->discovered; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(l>=UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_INSTALLED) ++s->installed; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_ENABLED||l==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STARTING||l==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING||l==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPING) ++s->enabled; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING) ++s->running; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_FAILED) ++s->failed; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_QUARANTINED) ++s->quarantined; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(update) ++s->updates_available; ++s->revision; }
/*
 * Provide the plugin extension host extension manager state health operation used by this
 * module and its client applications.
 */
UmiPluginExtensionHostHealthState umi_plugin_extension_host_extension_manager_state_health(const UmiPluginExtensionHostExtensionManagerState *s) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL) return UMI_PLUGIN_EXTENSION_HOST_HEALTH_UNKNOWN; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->quarantined>0U||s->failed*2U>s->discovered) return UMI_PLUGIN_EXTENSION_HOST_HEALTH_UNHEALTHY; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->failed>0U) return UMI_PLUGIN_EXTENSION_HOST_HEALTH_DEGRADED; return UMI_PLUGIN_EXTENSION_HOST_HEALTH_HEALTHY; }
