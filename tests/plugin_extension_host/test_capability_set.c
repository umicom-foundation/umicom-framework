/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_capability_set.c
 *
 * PURPOSE:
 *   Exercise maintain bounded capability bitsets and test subset compatibility.
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
#include "umicom/plugin/extension_host/capability_set.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostCapabilitySet s,r; umi_plugin_extension_host_capability_set_init(&s); umi_plugin_extension_host_capability_set_init(&r); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_capability_set_add(&s,129U)!=UMI_STATUS_OK||umi_plugin_extension_host_capability_set_add(&r,129U)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_plugin_extension_host_capability_set_contains(&s,129U)||!umi_plugin_extension_host_capability_set_contains_all(&s,&r)) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_capability_set_remove(&s,129U)!=UMI_STATUS_OK||umi_plugin_extension_host_capability_set_contains(&s,129U)) return 3; return 0; }
