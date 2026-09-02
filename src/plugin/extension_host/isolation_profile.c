/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/isolation_profile.c
 *
 * PURPOSE:
 *   Describe process/isolation requirements selected for one extension.
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
#include "umicom/plugin/extension_host/isolation_profile.h"
/*
 * Initialise plugin extension host isolation profile from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_isolation_profile_init(UmiPluginExtensionHostIsolationProfile *p) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL) { p->mode=UMI_PLUGIN_EXTENSION_HOST_ISOLATION_RESTRICTED_PROCESS; p->network_isolated=1; p->filesystem_isolated=1; p->process_isolated=1; p->environment_sanitized=1; p->risk_ceiling=100U; } }
/*
 * Provide the plugin extension host isolation profile configure operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_isolation_profile_configure(UmiPluginExtensionHostIsolationProfile *p,UmiPluginExtensionHostIsolation mode,uint32_t risk) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||risk>100U) return UMI_STATUS_INVALID_ARGUMENT; umi_plugin_extension_host_isolation_profile_init(p); p->mode=mode; p->risk_ceiling=risk; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(mode==UMI_PLUGIN_EXTENSION_HOST_ISOLATION_IN_PROCESS) { p->network_isolated=0; p->filesystem_isolated=0; p->process_isolated=0; } return UMI_STATUS_OK; }
/*
 * Provide the plugin extension host isolation profile satisfies operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_isolation_profile_satisfies(const UmiPluginExtensionHostIsolationProfile *p,uint32_t risk,int needs_network,int needs_process) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||risk>p->risk_ceiling) return 0; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(needs_network&&p->network_isolated) return 0; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(needs_process&&p->process_isolated) return 0; return 1; }
