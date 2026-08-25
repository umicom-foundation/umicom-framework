/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/host_process_policy.c
 *
 * PURPOSE:
 *   Decide when extensions require process isolation and restart limits.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/host_process_policy.h"
void umi_plugin_extension_host_host_process_policy_init(UmiPluginExtensionHostHostProcessPolicy *p) { if(p!=NULL) { p->maximum_crashes=3U; p->maximum_restarts=5U; p->isolation_risk_threshold=30U; p->require_isolation_for_untrusted=1; } }
UmiPluginExtensionHostIsolation umi_plugin_extension_host_host_process_policy_isolation(const UmiPluginExtensionHostHostProcessPolicy *p,UmiPluginExtensionHostTrust trust,uint32_t risk) { if(p==NULL) return UMI_PLUGIN_EXTENSION_HOST_ISOLATION_RESTRICTED_PROCESS; if(trust<=UMI_PLUGIN_EXTENSION_HOST_TRUST_UNKNOWN||(p->require_isolation_for_untrusted&&trust==UMI_PLUGIN_EXTENSION_HOST_TRUST_UNTRUSTED)||risk>=p->isolation_risk_threshold) return UMI_PLUGIN_EXTENSION_HOST_ISOLATION_RESTRICTED_PROCESS; return risk==0U&&trust==UMI_PLUGIN_EXTENSION_HOST_TRUST_SYSTEM?UMI_PLUGIN_EXTENSION_HOST_ISOLATION_IN_PROCESS:UMI_PLUGIN_EXTENSION_HOST_ISOLATION_PROCESS; }
int umi_plugin_extension_host_host_process_policy_may_restart(const UmiPluginExtensionHostHostProcessPolicy *p,uint32_t crashes,uint32_t restarts) { return p!=NULL&&crashes<=p->maximum_crashes&&restarts<p->maximum_restarts; }
