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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/lifecycle_transition.h"
int umi_plugin_extension_host_lifecycle_transition_allowed(UmiPluginExtensionHostLifecycle c,UmiPluginExtensionHostLifecycle n) { if(c==n) return 1; if(c==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_DISCOVERED&&n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_VERIFIED) return 1; if(c==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_VERIFIED&&n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_INSTALLED) return 1; if(c==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_INSTALLED&&(n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_ENABLED||n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPED)) return 1; if(c==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_ENABLED&&n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STARTING) return 1; if(c==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STARTING&&(n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING||n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_FAILED)) return 1; if(c==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING&&(n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPING||n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_FAILED)) return 1; if(c==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPING&&(n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPED||n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_FAILED)) return 1; if(c==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_FAILED&&(n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STARTING||n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_QUARANTINED||n==UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPED)) return 1; return 0; }
UmiStatus umi_plugin_extension_host_lifecycle_transition_validate(UmiPluginExtensionHostLifecycle c,UmiPluginExtensionHostLifecycle n) { return umi_plugin_extension_host_lifecycle_transition_allowed(c,n)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE; }
