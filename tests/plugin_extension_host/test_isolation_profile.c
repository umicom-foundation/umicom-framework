/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_isolation_profile.c
 *
 * PURPOSE:
 *   Exercise describe process/isolation requirements selected for one extension.
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
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostIsolationProfile p; umi_plugin_extension_host_isolation_profile_init(&p); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_isolation_profile_satisfies(&p,20U,1,0)) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_isolation_profile_configure(&p,UMI_PLUGIN_EXTENSION_HOST_ISOLATION_PROCESS,50U)!=UMI_STATUS_OK) return 2; return 0; }
