/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/contribution_activation.c
 *
 * PURPOSE:
 *   Evaluate whether a contribution may activate in the current host state.
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
#include "umicom/plugin/extension_host/contribution_activation.h"
/*
 * Provide the plugin extension host contribution activation evaluate operation used by
 * this module and its client applications.
 */
UmiPluginExtensionHostDecision umi_plugin_extension_host_contribution_activation_evaluate(const UmiPluginExtensionHostContributionActivation *v) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(v==NULL||v->quarantined||!v->plugin_enabled||!v->dependency_ready||!v->extension_point_ready||v->risk>80U) return UMI_PLUGIN_EXTENSION_HOST_DENY; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!v->permission_ready||v->risk>40U) return UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL; return UMI_PLUGIN_EXTENSION_HOST_ALLOW; }
