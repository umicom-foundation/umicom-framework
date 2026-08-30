/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_lifecycle_supervision.c
 *
 * PURPOSE:
 *   Exercise choose lifecycle supervision actions following crash, timeout or policy failure.
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
#include "umicom/plugin/extension_host/lifecycle_supervision.h"
int main(void) { UmiPluginExtensionHostLifecycleSupervision v={0U,0U,3U,0,0,1}; if(umi_plugin_extension_host_lifecycle_supervision_decide(&v)!=UMI_PLUGIN_EXTENSION_HOST_SUPERVISION_RESTART) return 1; v.trust_failure=1; if(umi_plugin_extension_host_lifecycle_supervision_decide(&v)!=UMI_PLUGIN_EXTENSION_HOST_SUPERVISION_QUARANTINE) return 2; return 0; }
