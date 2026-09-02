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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/host_reconnect.h"
/*
 * Initialise plugin extension host host reconnect from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_host_reconnect_init(UmiPluginExtensionHostHostReconnect *v) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(v!=NULL) { v->attempts=0U; v->maximum_attempts=6U; v->base_delay_ms=250U; v->maximum_delay_ms=10000U; } }
/*
 * Provide the plugin extension host host reconnect failed operation used by this module
 * and its client applications.
 */
UmiStatus umi_plugin_extension_host_host_reconnect_failed(UmiPluginExtensionHostHostReconnect *v) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(v==NULL) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(v->attempts>=v->maximum_attempts) return UMI_STATUS_UNAVAILABLE; ++v->attempts; return UMI_STATUS_OK; }
/*
 * Provide the plugin extension host host reconnect delay ms operation used by this module
 * and its client applications.
 */
uint64_t umi_plugin_extension_host_host_reconnect_delay_ms(const UmiPluginExtensionHostHostReconnect *v) { uint64_t d; uint32_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(v==NULL) return 0U; d=v->base_delay_ms; /* Visit each bounded item once so every record receives the same rule. */ for(i=1U;i<v->attempts&&d<v->maximum_delay_ms;++i) d=d>v->maximum_delay_ms/2U?v->maximum_delay_ms:d*2U; return d>v->maximum_delay_ms?v->maximum_delay_ms:d; }
/*
 * Provide the plugin extension host host reconnect may retry operation used by this module
 * and its client applications.
 */
int umi_plugin_extension_host_host_reconnect_may_retry(const UmiPluginExtensionHostHostReconnect *v) { return v!=NULL&&v->attempts<v->maximum_attempts; }
