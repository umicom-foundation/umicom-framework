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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/host_process_health.h"
uint32_t umi_plugin_extension_host_host_process_health_score(const UmiPluginExtensionHostHostProcessHealth *v,uint64_t timeout) { int score=100; uint64_t age; if(v==NULL||timeout==0U) return 0U; age=v->now_ms>=v->last_heartbeat_ms?v->now_ms-v->last_heartbeat_ms:0U; if(!v->responsive) score-=50; if(age>timeout) score-=40; score-=(int)(v->crash_count*10U>30U?30U:v->crash_count*10U); score-=(int)(v->restart_count*5U>20U?20U:v->restart_count*5U); return score<0?0U:(uint32_t)score; }
UmiPluginExtensionHostHealthState umi_plugin_extension_host_host_process_health_evaluate(const UmiPluginExtensionHostHostProcessHealth *v,uint64_t timeout) { uint32_t s=umi_plugin_extension_host_host_process_health_score(v,timeout); if(s>=80U) return UMI_PLUGIN_EXTENSION_HOST_HEALTH_HEALTHY; if(s>=50U) return UMI_PLUGIN_EXTENSION_HOST_HEALTH_DEGRADED; return UMI_PLUGIN_EXTENSION_HOST_HEALTH_UNHEALTHY; }
