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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/host_process_policy.h"
/*
 * Initialise plugin extension host host process policy from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_host_process_policy_init(UmiPluginExtensionHostHostProcessPolicy *p) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL) { p->maximum_crashes=3U; p->maximum_restarts=5U; p->isolation_risk_threshold=30U; p->require_isolation_for_untrusted=1; } }
/*
 * Provide the plugin extension host host process policy isolation operation used by this
 * module and its client applications.
 */
UmiPluginExtensionHostIsolation umi_plugin_extension_host_host_process_policy_isolation(const UmiPluginExtensionHostHostProcessPolicy *p,UmiPluginExtensionHostTrust trust,uint32_t risk) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL) return UMI_PLUGIN_EXTENSION_HOST_ISOLATION_RESTRICTED_PROCESS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(trust<=UMI_PLUGIN_EXTENSION_HOST_TRUST_UNKNOWN||(p->require_isolation_for_untrusted&&trust==UMI_PLUGIN_EXTENSION_HOST_TRUST_UNTRUSTED)||risk>=p->isolation_risk_threshold) return UMI_PLUGIN_EXTENSION_HOST_ISOLATION_RESTRICTED_PROCESS; return risk==0U&&trust==UMI_PLUGIN_EXTENSION_HOST_TRUST_SYSTEM?UMI_PLUGIN_EXTENSION_HOST_ISOLATION_IN_PROCESS:UMI_PLUGIN_EXTENSION_HOST_ISOLATION_PROCESS; }
/*
 * Provide the plugin extension host host process policy may restart operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_host_process_policy_may_restart(const UmiPluginExtensionHostHostProcessPolicy *p,uint32_t crashes,uint32_t restarts) { return p!=NULL&&crashes<=p->maximum_crashes&&restarts<p->maximum_restarts; }
