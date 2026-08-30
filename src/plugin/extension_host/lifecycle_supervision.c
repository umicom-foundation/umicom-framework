/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/lifecycle_supervision.c
 *
 * PURPOSE:
 *   Choose lifecycle supervision actions following crash, timeout or policy failure.
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
#include "umicom/plugin/extension_host/lifecycle_supervision.h"
UmiPluginExtensionHostSupervisionAction umi_plugin_extension_host_lifecycle_supervision_decide(const UmiPluginExtensionHostLifecycleSupervision *v) { if(v==NULL) return UMI_PLUGIN_EXTENSION_HOST_SUPERVISION_DISABLE; if(v->trust_failure||v->policy_failure||v->crash_count>=3U) return UMI_PLUGIN_EXTENSION_HOST_SUPERVISION_QUARANTINE; if(v->heartbeat_timeout&&v->restart_count<v->maximum_restarts) return UMI_PLUGIN_EXTENSION_HOST_SUPERVISION_RESTART; if(v->restart_count>=v->maximum_restarts) return UMI_PLUGIN_EXTENSION_HOST_SUPERVISION_DISABLE; return UMI_PLUGIN_EXTENSION_HOST_SUPERVISION_NONE; }
