/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/host_reconnect.c
 *
 * PURPOSE:
 *   Calculate bounded reconnect backoff following host interruption.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/host_reconnect.h"
void umi_plugin_extension_host_host_reconnect_init(UmiPluginExtensionHostHostReconnect *v) { if(v!=NULL) { v->attempts=0U; v->maximum_attempts=6U; v->base_delay_ms=250U; v->maximum_delay_ms=10000U; } }
UmiStatus umi_plugin_extension_host_host_reconnect_failed(UmiPluginExtensionHostHostReconnect *v) { if(v==NULL) return UMI_STATUS_INVALID_ARGUMENT; if(v->attempts>=v->maximum_attempts) return UMI_STATUS_UNAVAILABLE; ++v->attempts; return UMI_STATUS_OK; }
uint64_t umi_plugin_extension_host_host_reconnect_delay_ms(const UmiPluginExtensionHostHostReconnect *v) { uint64_t d; uint32_t i; if(v==NULL) return 0U; d=v->base_delay_ms; for(i=1U;i<v->attempts&&d<v->maximum_delay_ms;++i) d=d>v->maximum_delay_ms/2U?v->maximum_delay_ms:d*2U; return d>v->maximum_delay_ms?v->maximum_delay_ms:d; }
int umi_plugin_extension_host_host_reconnect_may_retry(const UmiPluginExtensionHostHostReconnect *v) { return v!=NULL&&v->attempts<v->maximum_attempts; }
