/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_contribution_activation.c
 *
 * PURPOSE:
 *   Exercise evaluate whether a contribution may activate in the current host state.
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
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostContributionActivation v={1,1,1,1,0,20U}; /* Apply this operation only while the related capability or state is available. */ if(umi_plugin_extension_host_contribution_activation_evaluate(&v)!=UMI_PLUGIN_EXTENSION_HOST_ALLOW) return 1; v.permission_ready=0; /* Apply this operation only while the related capability or state is available. */ if(umi_plugin_extension_host_contribution_activation_evaluate(&v)!=UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL) return 2; v.quarantined=1; /* Apply this operation only while the related capability or state is available. */ if(umi_plugin_extension_host_contribution_activation_evaluate(&v)!=UMI_PLUGIN_EXTENSION_HOST_DENY) return 3; return 0; }
