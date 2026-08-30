/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/host_heartbeat.c
 *
 * PURPOSE:
 *   Track liveness deadlines for isolated extension-host sessions.
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
#include "umicom/plugin/extension_host/host_heartbeat.h"
void umi_plugin_extension_host_host_heartbeat_init(UmiPluginExtensionHostHostHeartbeat *h,uint64_t interval,uint64_t timeout) { if(h!=NULL) { h->interval_ms=interval; h->timeout_ms=timeout; h->last_sent_ms=0U; h->last_received_ms=0U; h->missed=0U; } }
void umi_plugin_extension_host_host_heartbeat_sent(UmiPluginExtensionHostHostHeartbeat *h,uint64_t now) { if(h!=NULL) h->last_sent_ms=now; }
void umi_plugin_extension_host_host_heartbeat_received(UmiPluginExtensionHostHostHeartbeat *h,uint64_t now) { if(h!=NULL) { h->last_received_ms=now; h->missed=0U; } }
int umi_plugin_extension_host_host_heartbeat_expired(const UmiPluginExtensionHostHostHeartbeat *h,uint64_t now) { return h==NULL||h->timeout_ms==0U||now<h->last_received_ms||now-h->last_received_ms>h->timeout_ms; }
