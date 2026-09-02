/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/host_process_health.c
 *
 * PURPOSE:
 *   Calculate isolated extension-host process health from heartbeat and crash evidence.
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
#include "umicom/plugin/extension_host/host_process_health.h"
/*
 * Provide the plugin extension host host process health score operation used by this
 * module and its client applications.
 */
uint32_t umi_plugin_extension_host_host_process_health_score(const UmiPluginExtensionHostHostProcessHealth *v,uint64_t timeout) { int score=100; uint64_t age; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(v==NULL||timeout==0U) return 0U; age=v->now_ms>=v->last_heartbeat_ms?v->now_ms-v->last_heartbeat_ms:0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!v->responsive) score-=50; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(age>timeout) score-=40; score-=(int)(v->crash_count*10U>30U?30U:v->crash_count*10U); score-=(int)(v->restart_count*5U>20U?20U:v->restart_count*5U); return score<0?0U:(uint32_t)score; }
/*
 * Provide the plugin extension host host process health evaluate operation used by this
 * module and its client applications.
 */
UmiPluginExtensionHostHealthState umi_plugin_extension_host_host_process_health_evaluate(const UmiPluginExtensionHostHostProcessHealth *v,uint64_t timeout) { uint32_t s=umi_plugin_extension_host_host_process_health_score(v,timeout); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s>=80U) return UMI_PLUGIN_EXTENSION_HOST_HEALTH_HEALTHY; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s>=50U) return UMI_PLUGIN_EXTENSION_HOST_HEALTH_DEGRADED; return UMI_PLUGIN_EXTENSION_HOST_HEALTH_UNHEALTHY; }
