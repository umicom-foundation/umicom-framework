/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/lifecycle_transition.c
 *
 * PURPOSE:
 *   Validate explicit extension lifecycle transitions.
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
#include "umicom/plugin/extension_host/lifecycle_transition.h"
/*
 * Provide the plugin extension host lifecycle transition allowed operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_lifecycle_transition_allowed(UmiPluginExtensionHostLifecycle c,UmiPluginExtensionHostLifecycle n) { /* Preserve the original failure result so the caller can respond to the correct cause. */ if(c==n) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(c==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_DISCOVERED&&n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_VERIFIED) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(c==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_VERIFIED&&n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_INSTALLED) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(c==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_INSTALLED&&(n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_ENABLED||n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPED)) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(c==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_ENABLED&&n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STARTING) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(c==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STARTING&&(n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING||n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_FAILED)) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(c==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING&&(n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPING||n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_FAILED)) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(c==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPING&&(n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPED||n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_FAILED)) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(c==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_FAILED&&(n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STARTING||n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_QUARANTINED||n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPED)) return 1; return 0; }
/*
 * Check that plugin extension host lifecycle transition satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_plugin_extension_host_lifecycle_transition_validate(UmiPluginExtensionHostLifecycle c,UmiPluginExtensionHostLifecycle n) { return umi_plugin_extension_host_lifecycle_transition_allowed(c,n)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE; }
